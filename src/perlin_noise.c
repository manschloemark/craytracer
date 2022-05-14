#include "perlin_noise.h"

#include "stdio.h"

float FloorFloat(float n) {
	return (n > 0.0) ? (float)(int)n : (float)((int) (n - 1.0));
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


float perlin_noise(perlin *perl, point3 *pt) {
	float u = pt->x - FloorFloat(pt->x);
	float v = pt->y - FloorFloat(pt->y);
	float w = pt->z - FloorFloat(pt->z);
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


float perlin_interpolate_with_derivative(vec3 c[2][2][2], float u, float v, float w, vec3 *d) {
	float uu = u * u * (3.0 - 2.0*u);
	float vv = v * v * (3.0 - 2.0*v);
	float ww = w * w * (3.0 - 2.0*w);

	float d_uu = u * 6.0 * (1.0 - u);
	float d_vv = v * 6.0 * (1.0 - v);
	float d_ww = w * 6.0 * (1.0 - w);

	float accumulation = 0.0;

	for (int i = 0; i < 2; ++i) {
		float fi = (float)i;
		for (int j = 0; j < 2; ++j) {
			float fj = (float)j;
			for (int k = 0; k < 2; ++k) {
				float fk = (float)k;
				vec3 weight = vec3_new(u - fi, v - fj, w - fk);
				float dot = vec3_dot(c[i][j][k], weight);
				float l_u = (fi * uu + (1.0 - fi) * (1.0 - uu));
				float l_v = (fj * vv + (1.0 - fj) * (1.0 - vv));
				float l_w = (fk * ww + (1.0 - fk) * (1.0 - ww));
				accumulation += l_u * l_v * l_w * dot;
				vec3 cur_d = vec3_mul(vec3_new(fi * d_uu * l_v * l_w, fj * d_vv * l_u * l_w, fk * d_ww * l_u * l_v), 2.0 * dot);
				*d = vec3_add(*d, cur_d);
			}
		}
	}
	return accumulation;
}

float shirley_perlin_noise_with_derivative(perlin *perl, point3 *pt, vec3 *d) {
	float u = pt->x - FloorFloat(pt->x);
	float v = pt->y - FloorFloat(pt->y);
	float w = pt->z - FloorFloat(pt->z);
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
	float result = perlin_interpolate_with_derivative(c, u, v, w, d);
	return result;
}

static inline float LERP(float a, float b, float c) {
	return a + (b - a) * c;

}

// NOTE : this might be wrong. Specifically vectors with names vXXX
float perlin_noise_with_derivative(perlin *perl, point3 *pt, vec3 *d) {
	float u = pt->x - FloorFloat(pt->x);
	float v = pt->y - FloorFloat(pt->y);
	float w = pt->z - FloorFloat(pt->z);
	int i = (int)FloorInt(pt->x);
	int j = (int)FloorInt(pt->y);
	int k = (int)FloorInt(pt->z);

	int max_index = perl->pointcount - 1;


	vec3 v000 = perl->random_vectors[
					perl->perm_x[(i+0) & max_index] ^
					perl->perm_y[(j+0) & max_index] ^
					perl->perm_z[(k+0) & max_index]];
	vec3 v001 = perl->random_vectors[
					perl->perm_x[(i+0) & max_index] ^
					perl->perm_y[(j+0) & max_index] ^
					perl->perm_z[(k+1) & max_index]];
	vec3 v010 = perl->random_vectors[
					perl->perm_x[(i+0) & max_index] ^
					perl->perm_y[(j+1) & max_index] ^
					perl->perm_z[(k+0) & max_index]];
	vec3 v011 = perl->random_vectors[
					perl->perm_x[(i+0) & max_index] ^
					perl->perm_y[(j+1) & max_index] ^
					perl->perm_z[(k+1) & max_index]];
	vec3 v100 = perl->random_vectors[
					perl->perm_x[(i+1) & max_index] ^
					perl->perm_y[(j+0) & max_index] ^
					perl->perm_z[(k+0) & max_index]];
	vec3 v101 = perl->random_vectors[
					perl->perm_x[(i+1) & max_index] ^
					perl->perm_y[(j+0) & max_index] ^
					perl->perm_z[(k+1) & max_index]];
	vec3 v110 = perl->random_vectors[
					perl->perm_x[(i+1) & max_index] ^
					perl->perm_y[(j+1) & max_index] ^
					perl->perm_z[(k+0) & max_index]];
	vec3 v111 = perl->random_vectors[
					perl->perm_x[(i+1) & max_index] ^
					perl->perm_y[(j+1) & max_index] ^
					perl->perm_z[(k+1) & max_index]];

	float uu = u * u * u * (u * (6.0 * u - 15.0) + 10.0);
	float vv = v * v * v * (v * (6.0 * v - 15.0) + 10.0);
	float ww = w * w * w * (w * (6.0 * w - 15.0) + 10.0);

	float d_uu = u * u * u * (u * u * (u - 2.0) + 1.0);
	float d_vv = v * v * v * (v * v * (v - 2.0) + 1.0);
	float d_ww = w * w * w * (w * w * (w - 2.0) + 1.0);

	float dot000 = vec3_dot(v000, vec3_new(u, v, w));
	float dot100 = vec3_dot(v100, vec3_new(u - 1.0, v, w));
	float dot010 = vec3_dot(v010, vec3_new(u, v - 1.0, w));
	float dot110 = vec3_dot(v110, vec3_new(u - 1.0, v - 1.0, w));
	float dot001 = vec3_dot(v001, vec3_new(u, v, w - 1.0));
	float dot101 = vec3_dot(v101, vec3_new(u - 1.0, v, w - 1.0));
	float dot011 = vec3_dot(v011, vec3_new(u, v - 1.0, w - 1.0));
	float dot111 = vec3_dot(v111, vec3_new(u - 1.0, v - 1.0, w - 1.0));

	float result = LERP(LERP(LERP(dot000, dot100, uu), LERP(dot010, dot110, uu), vv), LERP(LERP(dot001, dot101, uu), LERP(dot011, dot111, uu), vv), ww);

	d->x = v000.x + d_uu*(dot100 - dot000) + uu*(v100.x-v000.x) + vv*(v010.x - v000.x) + ww*(v001.x-v000.x) + d_uu*vv*(dot110-dot010-dot100+dot000) + uu*vv*(v110.x-v010.x-v100.x+v000.x) + d_uu*ww*(dot101-dot001-dot100+dot000) + uu*ww*(v101.x-v001.x-v100.x-v000.x) + vv*ww*(v011.x-v001.x-v010.x+v000.x) + d_uu*vv*ww*(dot111-dot011-dot101+dot001-dot110+dot010+dot100-dot000) + uu*vv*ww*(v111.x-v011.x-v101.x+v001.x-v110.x+v010.x+v100.x-v000.x);

	d->y = v000.y + uu*(v100.y-v000.y) + d_vv*(dot010-dot000) + vv*(v010.y - v000.y) + ww*(v001.y-v000.y) + uu*d_vv*(dot110-dot010-dot100+dot000) + uu*vv*(v110.y-v010.y-v100.y+v000.y) + uu*ww*(v101.y-v001.y-v100.y+v000.y) + d_vv*ww*(dot011-dot001-dot010+dot000) + vv*ww*(v011.y-v001.y-v010.y+v000.y) + uu*d_vv*ww*(dot111-dot011-dot101+dot001-dot110+dot010+dot100-dot000) + uu*vv*ww*(v111.y-v011.y-v101.y+v001.y-v110.y+v010.y+v100.y-v000.y);

	d->z = v000.z + uu*(v100.z-v000.z) + vv*(v010.z-v000.z) + d_ww*(dot001-dot000) + ww*(v001.z-v000.z) + uu*vv*(v110.z-v010.z-v100.z+v000.z) + uu*d_ww*(dot101-dot001-dot100+dot000) + uu*ww*(v101.z-v001.z-v100.z+v000.z) + vv*d_ww*(dot011-dot001-dot010+dot000) + vv*ww*(v011.z-v001.z-v010.z+v000.z) + uu*vv*d_ww*(dot111-dot011-dot101+dot001-dot110+dot010+dot100-dot000) + uu*vv*ww*(v111.z-v011.z-v101.z+v001.z-v110.z+v010.z+v100.z-v000.z);

	return result;
}

/* FBM - Fractional Brownian Motion -- this is something that uses any sort of random process - not necessarily perlin noise. Maybe there's a better place for this? */
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

float fbm_with_derivative(perlin *perlin, vec3 v, float h, int octaves, vec3 *d) {
	vec3 derivative = vec3_new(0.0, 0.0, 0.0);
	float g = exp2(-h);
	float f = 1.0;
	float a = 1.0;
	float t = 0.0;

	for (int i = 0; i < octaves; ++i) {
		vec3 vp = vec3_mul(v, f);
		t += a * perlin_noise_with_derivative(perlin, &vp, &derivative);
		f *= 2.0;
		d->x += a * derivative.x;
		d->y += a * derivative.y;
		d->z += a * derivative.z;
		a *= g;
	}
	*d = derivative;
	return t;
}
