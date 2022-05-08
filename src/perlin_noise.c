#include "perlin_noise.h"

#include "stdio.h"

float Floor(float n) {
	return (n > 0.0) ? (float)(int)n : (float)((int) n - 1.0);
}
int FloorInt(float n) {
	return (n > 0.0) ? (int)n : (int)n - 1;
}

void perlin_gen_perm(int *perm, int pointcount) {
	int i;
	// Fill array with 0 through POINTCOUNT
	for (i = 0; i < pointcount; ++i) {
		perm[i] = i;
	}
	// Randomly swap numbers
	for(i = pointcount - 1; i > 0; --i) {
		int swap_index = (int)(random_float_between(0.0, (float)i+1.0));
		int temp = perm[i];
		perm[i] = perm[swap_index];
		perm[swap_index] = temp;
	}
}

// WARNING : this mallocs things that are 100% intented to be
// copied to a memory_region using memory_region_add()
perlin perlin_new(int pointcount) {
	perlin p = {};
	p.pointcount = pointcount;
	p.random_vectors = (vec3 *)malloc(sizeof(vec3) * p.pointcount);
	p.perm_x = (int *)malloc(sizeof(int) * p.pointcount);
	p.perm_y = (int *)malloc(sizeof(int) * p.pointcount);
	p.perm_z = (int *)malloc(sizeof(int) * p.pointcount);
	int i;
	for (i = 0; i < p.pointcount; ++i) {
		p.random_vectors[i] = vec3_unit(vec3_bounded_random(-1.0, 1.0));
	}

	perlin_gen_perm(p.perm_x, p.pointcount);
	perlin_gen_perm(p.perm_y, p.pointcount);
	perlin_gen_perm(p.perm_z, p.pointcount);

	return p;
}

perlin *add_perlin(memory_region *region, int pointcount) {
	perlin p = perlin_new(pointcount);
	// Copy every array into the memory region
	// This ends up taking A lot of memory compared to everything else
	// (sizeof(vec3) * 256) + 3 * (sizeof(int) * 256) ~~ 3 kilobytes
	vec3 *vec3_to_free = p.random_vectors;
	p.random_vectors = (vec3 *)memory_region_add(region, vec3_to_free, sizeof(*p.random_vectors) * p.pointcount);
	free(vec3_to_free);

	int *to_free_x = p.perm_x;
	p.perm_x = (int *)memory_region_add(region, to_free_x, sizeof(*p.perm_x) * p.pointcount);
	free(to_free_x);

	int *to_free_y = p.perm_y;
	p.perm_y = (int *)memory_region_add(region, to_free_y, sizeof(*p.perm_y) * p.pointcount);
	free(to_free_y);

	int *to_free_z = p.perm_z;
	p.perm_z = (int *)memory_region_add(region, to_free_z, sizeof(*p.perm_z) * p.pointcount);
	free(to_free_z);

	perlin *perlptr = (perlin *)memory_region_add(region, &p, sizeof(p));

	return perlptr;
}

float perlin_interpolate(vec3 c[2][2][2], float u, float v, float w) {
	float uu = u * u * (3.0 - 2.0*u);
	float vv = v * v * (3.0 - 2.0*v);
	float ww = w * w * (3.0 - 2.0*w);
	float accumulation = 0.0;

	for (int i = 0; i < 2; ++i) {
		float fi = (float)i;
		for (int j = 0; j < 2; ++j) {
			float fj = (float)j;
			for (int k = 0; k < 2; ++k) {
				float fk = (float)k;
				vec3 weight = vec3_new(u - fi, v - fj, w - fk);
				accumulation += (fi * uu + (1.0 - fi) * (1.0 - uu)) *
												(fj * vv + (1.0 - fj) * (1.0 - vv)) *
												(fk * ww + (1.0 - fk) * (1.0 - ww)) *
												vec3_dot(c[i][j][k], weight);
			}
		}
	}
	return accumulation;
}

float perlin_noise(perlin *perl, point3 *pt) {
	float u = pt->x - floorf(pt->x);
	float v = pt->y - floorf(pt->y);
	float w = pt->z - floorf(pt->z);
	int i = (int)FloorInt(pt->x);
	int j = (int)FloorInt(pt->y);
	int k = (int)FloorInt(pt->z);

	vec3 c[2][2][2];

	int max_index = perl->pointcount - 1;

	for (int di = 0; di < 2; ++di) {
		for (int dj = 0; dj < 2; ++dj) {
			for (int dk = 0; dk < 2; ++dk) {
				c[di][dj][dk] = perl->random_vectors[
					perl->perm_x[(i+di) & max_index] ^
					perl->perm_y[(j+dj) & max_index] ^
					perl->perm_z[(k+dk) & max_index]];
			}
		}
	}
	float result = perlin_interpolate(c, u, v, w);
	return result;
}

float perlin_turbulence(perlin *perl, point3 *pt, int depth) {
	float accumulation = 0.0;
	float weight = 1.0;
	point3 temp_pt = vec3_new(pt->x, pt->y, pt->z);
	
	for (int i = 0; i < depth; ++i) {
		accumulation += weight * perlin_noise(perl, &temp_pt);
		weight *= 0.5;
		temp_pt = vec3_mul(temp_pt, 2.0);
	}
	return fabs(accumulation);
}

float fbm(perlin *perlin, vec3 v, float h, int octaves) {
	float g = exp2(-h);
	float f = 1.0;
	float a = 1.0;
	float t = 0.0;

	for (int i = 0; i < octaves; ++i) {
		vec3 vp = vec3_mul(v, f);
		t += a * perlin_noise(perlin, &vp);
		f *= 2.0;
		a *= g;
	}
	return t;
}
