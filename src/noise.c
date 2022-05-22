#include "noise.h"

#include "stdio.h"

static inline float LERP(float a, float b, float c) {
	return a + (b - a) * c;
}

float FloorFloat(float n) {
	return (n > 0.0f) ? (float)(int)n : (float)((int) (n - 1.0f));
}
int FloorInt(float n) {
	return (int)((n > 0.0f) ? n : n - 1.0f);
}

void permute(int *perm, int pointcount, thread_context *thread) {
	int i;
	// Fill array with 0 through POINTCOUNT
	for (i = 0; i < pointcount; ++i) {
		perm[i] = i;
	}
	// Randomly swap numbers
	for(i = pointcount - 1; i > 0; --i) {
		int swap_index = (int)(random_float_between(0.0f, (float)i+1.0f, &thread->rand_state));
		int temp = perm[i];
		perm[i] = perm[swap_index];
		perm[swap_index] = temp;
	}
}

// WARNING : this mallocs things that are 100% intented to be
// copied to a memory_region using memory_region_add()
perlin perlin_new(int pointcount, thread_context *thread) {
	perlin p = {};
	p.pointcount = pointcount;
	p.random_vectors = (vec3 *)malloc(sizeof(vec3) * p.pointcount);
	p.perm_x = (int *)malloc(sizeof(int) * p.pointcount);
	p.perm_y = (int *)malloc(sizeof(int) * p.pointcount);
	p.perm_z = (int *)malloc(sizeof(int) * p.pointcount);
	int i;
	for (i = 0; i < p.pointcount; ++i) {
		p.random_vectors[i] = vec3_unit(vec3_bounded_random(-1.0f, 1.0f, &thread->rand_state));
	}

	permute(p.perm_x, p.pointcount, thread);
	permute(p.perm_y, p.pointcount, thread);
	permute(p.perm_z, p.pointcount, thread);

	return p;
}

perlin *add_perlin(memory_region *region, int pointcount, thread_context *thread) {
	perlin p = perlin_new(pointcount, thread);
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


	perlin *perlptr = (perlin *)memory_region_add(region, &p, sizeof(perlin));
	return perlptr;
}

noise *add_perlin_noise(memory_region *region, int pointcount, thread_context *thread) {
	noise n = {};
	n.Noise = (*perlin_noise);
	n.NoiseD = (*perlin_noise_with_derivative);
	perlin *perlptr = add_perlin(region, pointcount, thread);
	n.source = perlptr;
	noise *noiseptr = (noise *)memory_region_add(region, &n, sizeof(noise));
	return noiseptr;
}

float perlin_interpolate(vec3 c[2][2][2], float u, float v, float w) {
	float uu = u * u * (3.0f - 2.0f*u);
	float vv = v * v * (3.0f - 2.0f*v);
	float ww = w * w * (3.0f - 2.0f*w);
	float accumulation = 0.0f;

	for (int i = 0; i < 2; ++i) {
		float fi = (float)i;
		for (int j = 0; j < 2; ++j) {
			float fj = (float)j;
			for (int k = 0; k < 2; ++k) {
				float fk = (float)k;
				vec3 weight = vec3_new(u - fi, v - fj, w - fk);
				accumulation += (fi * uu + (1.0f - fi) * (1.0f - uu)) *
												(fj * vv + (1.0f - fj) * (1.0f - vv)) *
												(fk * ww + (1.0f - fk) * (1.0f - ww)) *
												vec3_dot(c[i][j][k], weight);
			}
		}
	}
	return accumulation;
}

float perlin_turbulence(perlin *perl, point3 *pt, int depth) {
	float accumulation = 0.0f;
	float weight = 1.0f;
	point3 temp_pt = vec3_new(pt->x, pt->y, pt->z);
	
	for (int i = 0; i < depth; ++i) {
		accumulation += weight * perlin_noise(perl, &temp_pt);
		weight *= 0.5f;
		temp_pt = vec3_mul(temp_pt, 2.0f);
	}
	return fabs(accumulation);
}


float shirley_perlin_noise(void *src, point3 *pt) {
	perlin *perl = (perlin *)src;
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


float perlin_noise(void *src, point3 *pt) {
	perlin *perl = (perlin *)src;
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

	float uu = u * u * u * (u * (6.0f * u - 15.0f) + 10.0f);
	float vv = v * v * v * (v * (6.0f * v - 15.0f) + 10.0f);
	float ww = w * w * w * (w * (6.0f * w - 15.0f) + 10.0f);

	float dot000 = vec3_dot(v000, vec3_new(u, v, w));
	float dot100 = vec3_dot(v100, vec3_new(u - 1.0f, v, w));
	float dot010 = vec3_dot(v010, vec3_new(u, v - 1.0f, w));
	float dot110 = vec3_dot(v110, vec3_new(u - 1.0f, v - 1.0f, w));
	float dot001 = vec3_dot(v001, vec3_new(u, v, w - 1.0f));
	float dot101 = vec3_dot(v101, vec3_new(u - 1.0f, v, w - 1.0f));
	float dot011 = vec3_dot(v011, vec3_new(u, v - 1.0f, w - 1.0f));
	float dot111 = vec3_dot(v111, vec3_new(u - 1.0f, v - 1.0f, w - 1.0f));

	float result = LERP(LERP(LERP(dot000, dot100, uu), LERP(dot010, dot110, uu), vv), LERP(LERP(dot001, dot101, uu), LERP(dot011, dot111, uu), vv), ww);
	return result;
}

float perlin_interpolate_with_derivative(vec3 c[2][2][2], float u, float v, float w, vec3 *d) {
	float uu = u * u * (3.0f - 2.0f*u);
	float vv = v * v * (3.0f - 2.0f*v);
	float ww = w * w * (3.0f - 2.0f*w);

	float d_uu = u * 6.0f * (1.0f - u);
	float d_vv = v * 6.0f * (1.0f - v);
	float d_ww = w * 6.0f * (1.0f - w);

	float accumulation = 0.0f;

	for (int i = 0; i < 2; ++i) {
		float fi = (float)i;
		for (int j = 0; j < 2; ++j) {
			float fj = (float)j;
			for (int k = 0; k < 2; ++k) {
				float fk = (float)k;
				vec3 weight = vec3_new(u - fi, v - fj, w - fk);
				float dot = vec3_dot(c[i][j][k], weight);
				float l_u = (fi * uu + (1.0f - fi) * (1.0f - uu));
				float l_v = (fj * vv + (1.0f - fj) * (1.0f - vv));
				float l_w = (fk * ww + (1.0f - fk) * (1.0f - ww));
				accumulation += l_u * l_v * l_w * dot;
				vec3 cur_d = vec3_mul(vec3_new(fi * d_uu * l_v * l_w, fj * d_vv * l_u * l_w, fk * d_ww * l_u * l_v), 2.0f * dot);
				*d = vec3_add(*d, cur_d);
			}
		}
	}
	return accumulation;
}

float shirley_perlin_noise_with_derivative(void *src, point3 *pt, vec3 *d) {
	perlin *perl = (perlin *)src;
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


// NOTE : this might be wrong. Specifically vectors with names vXXX
float perlin_noise_with_derivative(void *src, point3 *pt, vec3 *d) {
	perlin *perl = (perlin *)src;
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

	float uu = u * u * u * (u * (6.0f * u - 15.0f) + 10.0f);
	float vv = v * v * v * (v * (6.0f * v - 15.0f) + 10.0f);
	float ww = w * w * w * (w * (6.0f * w - 15.0f) + 10.0f);

	float d_uu = u * u * u * (u * u * (u - 2.0f) + 1.0f);
	float d_vv = v * v * v * (v * v * (v - 2.0f) + 1.0f);
	float d_ww = w * w * w * (w * w * (w - 2.0f) + 1.0f);

	float dot000 = vec3_dot(v000, vec3_new(u, v, w));
	float dot100 = vec3_dot(v100, vec3_new(u - 1.0f, v, w));
	float dot010 = vec3_dot(v010, vec3_new(u, v - 1.0f, w));
	float dot110 = vec3_dot(v110, vec3_new(u - 1.0f, v - 1.0f, w));
	float dot001 = vec3_dot(v001, vec3_new(u, v, w - 1.0f));
	float dot101 = vec3_dot(v101, vec3_new(u - 1.0f, v, w - 1.0f));
	float dot011 = vec3_dot(v011, vec3_new(u, v - 1.0f, w - 1.0f));
	float dot111 = vec3_dot(v111, vec3_new(u - 1.0f, v - 1.0f, w - 1.0f));

	float result = LERP(LERP(LERP(dot000, dot100, uu), LERP(dot010, dot110, uu), vv), LERP(LERP(dot001, dot101, uu), LERP(dot011, dot111, uu), vv), ww);

	d->x = v000.x + d_uu*(dot100 - dot000) + uu*(v100.x-v000.x) + vv*(v010.x - v000.x) + ww*(v001.x-v000.x) + d_uu*vv*(dot110-dot010-dot100+dot000) + uu*vv*(v110.x-v010.x-v100.x+v000.x) + d_uu*ww*(dot101-dot001-dot100+dot000) + uu*ww*(v101.x-v001.x-v100.x-v000.x) + vv*ww*(v011.x-v001.x-v010.x+v000.x) + d_uu*vv*ww*(dot111-dot011-dot101+dot001-dot110+dot010+dot100-dot000) + uu*vv*ww*(v111.x-v011.x-v101.x+v001.x-v110.x+v010.x+v100.x-v000.x);

	d->y = v000.y + uu*(v100.y-v000.y) + d_vv*(dot010-dot000) + vv*(v010.y - v000.y) + ww*(v001.y-v000.y) + uu*d_vv*(dot110-dot010-dot100+dot000) + uu*vv*(v110.y-v010.y-v100.y+v000.y) + uu*ww*(v101.y-v001.y-v100.y+v000.y) + d_vv*ww*(dot011-dot001-dot010+dot000) + vv*ww*(v011.y-v001.y-v010.y+v000.y) + uu*d_vv*ww*(dot111-dot011-dot101+dot001-dot110+dot010+dot100-dot000) + uu*vv*ww*(v111.y-v011.y-v101.y+v001.y-v110.y+v010.y+v100.y-v000.y);

	d->z = v000.z + uu*(v100.z-v000.z) + vv*(v010.z-v000.z) + d_ww*(dot001-dot000) + ww*(v001.z-v000.z) + uu*vv*(v110.z-v010.z-v100.z+v000.z) + uu*d_ww*(dot101-dot001-dot100+dot000) + uu*ww*(v101.z-v001.z-v100.z+v000.z) + vv*d_ww*(dot011-dot001-dot010+dot000) + vv*ww*(v011.z-v001.z-v010.z+v000.z) + uu*vv*d_ww*(dot111-dot011-dot101+dot001-dot110+dot010+dot100-dot000) + uu*vv*ww*(v111.z-v011.z-v101.z+v001.z-v110.z+v010.z+v100.z-v000.z);

	return result;
}

/* FBM - Fractional Brownian Motion -- this is something that uses any sort of random process - not necessarily perlin noise. Maybe there's a better place for this? */

/* SIMPLEX NOISE - Faster than Perlin noise and has some useful properties
 * I'm using this as a reference: https://weber.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf
 */

// NOTE : I'm still using the memory-expensive method of making three permutation arrays perm_x/y/z
//        there's probably a better way to do this.
// TODO : find a better way to get a random index like a hash function or something 

simplex simplex_new(int pointcount, thread_context *thread) {
	simplex s = {};
	s.skew = 1.0f / 3.0f;
	s.unskew = 1.0f / 6.0f;
	s.pointcount = pointcount;
	s.gradients = (vec3 *)malloc(s.pointcount * sizeof(vec3));
	for (int i = 0; i < s.pointcount; ++i) {
		s.gradients[i] = vec3_unit(vec3_bounded_random(-1.0f, 1.0f, &thread->rand_state));
	}

	s.perm_x = (int *)malloc(sizeof(int) * s.pointcount);
	s.perm_y = (int *)malloc(sizeof(int) * s.pointcount);
	s.perm_z = (int *)malloc(sizeof(int) * s.pointcount);

	permute(s.perm_x, s.pointcount, thread);
	permute(s.perm_y, s.pointcount, thread);
	permute(s.perm_z, s.pointcount, thread);

	return s;
}

simplex *add_simplex(memory_region *region, int gradient_count, thread_context *thread) {
	simplex s = simplex_new(gradient_count, thread);
	vec3 *vec3_to_free = s.gradients;
	s.gradients = (vec3 *)memory_region_add(region, vec3_to_free, sizeof(*s.gradients) * s.pointcount);
	free(vec3_to_free);

	int *to_free_x = s.perm_x;
	s.perm_x = (int *)memory_region_add(region, s.perm_x, sizeof(*s.perm_x) * s.pointcount);
	free(to_free_x);

	int *to_free_y = s.perm_y;
	s.perm_y = (int *)memory_region_add(region, s.perm_y, sizeof(*s.perm_y) * s.pointcount);
	free(to_free_y);

	int *to_free_z = s.perm_z;
	s.perm_z = (int *)memory_region_add(region, s.perm_z, sizeof(*s.perm_z) * s.pointcount);
	free(to_free_z);

	simplex *sptr = memory_region_add(region, &s, sizeof(simplex));
	return sptr;
}

noise *add_simplex_noise(memory_region *region, int gradient_count, thread_context *thread) {

	noise n = {};
	n.Noise = (*simplex_noise);
	n.NoiseD = (*simplex_noise_with_derivative);

	simplex *s = add_simplex(region, gradient_count, thread);
	n.source = s;
	noise *noiseptr = (noise *)memory_region_add(region, &n, sizeof(noise));
	return noiseptr;
}

float simplex_noise(void *src, vec3 *pt) {
	simplex *simp = (simplex *)src;
	float s = (pt->x + pt->y + pt->z) * simp->skew;
	int i = FloorInt(pt->x + s);
	int j = FloorInt(pt->y + s);
	int k = FloorInt(pt->z + s);

	float t = ((float)(i + j + k)) * simp->unskew;
	float u = ((float)i) - t;
	float v = ((float)j) - t;
	float w = ((float)k) - t;
	float x0 = pt->x - u;
	float y0 = pt->y - v;
	float z0 = pt->z - w;

	int i1, j1, k1, i2, j2, k2;
	if (x0 >= y0) {
		if (y0 >= z0) {
			i1=1; j1=0; k1=0;
			i2=1; j2=1; k2=0;
		} else if (x0 >= z0) {
			i1=1; j1=0; k1=0;
			i2=1; j2=0; k2=1;
		} else {
			i1=0; j1=0; k1=1;
			i2=1; j2=0; k2=1;
		}
	} else {
		if (y0 < z0) {
			i1=0; j1=0; k1=1;
			i2=0; j2=1; k2=1;
		} else if (x0 < z0) {
			i1=0; j1=1; k1=0;
			i2=0; j2=1; k2=1;
		} else {
			i1=0; j1=1; k1=0;
			i2=1; j2=1; k2=0;
		}
	} 

	float x1 = x0 - ((float)i1) + simp->unskew;
	float y1 = y0 - ((float)j1) + simp->unskew;
	float z1 = z0 - ((float)k1) + simp->unskew;
	float x2 = x0 - ((float)i2) + 2.0f*simp->unskew;
	float y2 = y0 - ((float)j2) + 2.0f*simp->unskew;
	float z2 = z0 - ((float)k2) + 2.0f*simp->unskew;
	float x3 = x0 - 1.0f + 3.0f*simp->unskew;
	float y3 = y0 - 1.0f + 3.0f*simp->unskew;
	float z3 = z0 - 1.0f + 3.0f*simp->unskew;

	int m = simp->pointcount - 1;
	int grad_index0 = simp->perm_x[i & m] ^ simp->perm_y[j & m] ^ simp->perm_z[k & m];
	int grad_index1 = simp->perm_x[(i+i1) & m] ^ simp->perm_y[(j+j1) & m] ^ simp->perm_z[(k+k1) & m];
	int grad_index2 = simp->perm_x[(i+i2) & m] ^ simp->perm_y[(j+j2) & m] ^ simp->perm_z[(k+k2) & m];
	int grad_index3 = simp->perm_x[(i+1) & m] ^ simp->perm_y[(j+1) & m] ^ simp->perm_z[(k+1) & m];

	vec3 g0 = simp->gradients[grad_index0];
	vec3 g1 = simp->gradients[grad_index1];
	vec3 g2 = simp->gradients[grad_index2];
	vec3 g3 = simp->gradients[grad_index3];

	float n0, n1, n2, n3;
	float dot0, dot1, dot2, dot3;
	float cont = 0.6f;
	float scale = 42.0f;

	float t0 = cont - x0*x0 - y0*y0 - z0*z0;
	if (t0 < 0.0f)
		n0 = 0.0f, dot0 = 0.0f;
	else {
		dot0 = vec3_dot(g0, vec3_new(x0, y0, z0));
		n0 = (t0*t0*t0*t0)*dot0;
	}

	float t1 = cont - x1*x1 - y1*y1 - z1*z1;
	if (t1 < 0.0f)
		n1 = 0.0f, dot1 = 0.0f;
	else
	{
		dot1 = vec3_dot(g1, vec3_new(x1, y1, z1));
		n1 = (t1*t1*t1*t1)*dot1;
	}

	float t2 = cont - x2*x2 - y2*y2 - z2*z2;
	if (t2 < 0.0f)
		n2 = 0.0f, dot2 = 0.0f;
	else
	{
		dot2 = vec3_dot(g2, vec3_new(x2, y2, z2));
		n2 = (t2*t2*t2*t2)*dot2;
	}

	float t3 = cont - x3*x3 - y3*y3 - z3*z3;
	if (t3 < 0.0f)
		n3 = 0.0f, dot3 = 0.0f;
	else
	{
		dot3 = vec3_dot(g3, vec3_new(x3, y3, z3));
		n3 = (t3*t3*t3*t3)*dot3;
	}

	float result =  scale * (n0 + n1 + n2 + n3);
	return result;
}

float simplex_noise_with_derivative(void *src, vec3 *pt, vec3 *d) {
	simplex *simp = (simplex *)src;

	float s = (pt->x + pt->y + pt->z) * simp->skew;
	int i = FloorInt(pt->x + s);
	int j = FloorInt(pt->y + s);
	int k = FloorInt(pt->z + s);

	float t = ((float)(i + j + k)) * simp->unskew;
	float u = ((float)i) - t;
	float v = ((float)j) - t;
	float w = ((float)k) - t;
	float x0 = pt->x - u;
	float y0 = pt->y - v;
	float z0 = pt->z - w;

	int i1, j1, k1, i2, j2, k2;
	if (x0 >= y0) {
		if (y0 >= z0) {
			i1=1; j1=0; k1=0;
			i2=1; j2=1; k2=0;
		} else if (x0 >= z0) {
			i1=1; j1=0; k1=0;
			i2=1; j2=0; k2=1;
		} else {
			i1=0; j1=0; k1=1;
			i2=1; j2=0; k2=1;
		}
	} else {
		if (y0 < z0) {
			i1=0; j1=0; k1=1;
			i2=0; j2=1; k2=1;
		} else if (x0 < z0) {
			i1=0; j1=1; k1=0;
			i2=0; j2=1; k2=1;
		} else {
			i1=0; j1=1; k1=0;
			i2=1; j2=1; k2=0;
		}
	} 

	float x1 = x0 - ((float)i1) + simp->unskew;
	float y1 = y0 - ((float)j1) + simp->unskew;
	float z1 = z0 - ((float)k1) + simp->unskew;
	float x2 = x0 - ((float)i2) + 2.0f*simp->unskew;
	float y2 = y0 - ((float)j2) + 2.0f*simp->unskew;
	float z2 = z0 - ((float)k2) + 2.0f*simp->unskew;
	float x3 = x0 - 1.0f + 3.0f*simp->unskew;
	float y3 = y0 - 1.0f + 3.0f*simp->unskew;
	float z3 = z0 - 1.0f + 3.0f*simp->unskew;

	int m = simp->pointcount - 1;
	int grad_index0 = simp->perm_x[i & m] ^ simp->perm_y[j & m] ^ simp->perm_z[k & m];
	int grad_index1 = simp->perm_x[(i+i1) & m] ^ simp->perm_y[(j+j1) & m] ^ simp->perm_z[(k+k1) & m];
	int grad_index2 = simp->perm_x[(i+i2) & m] ^ simp->perm_y[(j+j2) & m] ^ simp->perm_z[(k+k2) & m];
	int grad_index3 = simp->perm_x[(i+1) & m] ^ simp->perm_y[(j+1) & m] ^ simp->perm_z[(k+1) & m];
	vec3 g0 = simp->gradients[grad_index0];
	vec3 g1 = simp->gradients[grad_index1];
	vec3 g2 = simp->gradients[grad_index2];
	vec3 g3 = simp->gradients[grad_index3];

	float n0, n1, n2, n3;
	float dot0, dot1, dot2, dot3;
	vec3 d0 = vec3_new(0.0f, 0.0f, 0.0f), d1 = vec3_new(0.0f, 0.0f, 0.0f), d2 = vec3_new(0.0f, 0.0f, 0.0f), d3 = vec3_new(0.0f, 0.0f, 0.0f);
	float cont = 0.6f;
	float scale = 42.0f;

	float t0 = cont - x0*x0 - y0*y0 - z0*z0;
	if (t0 < 0.0f)
		n0 = 0.0f, dot0 = 0.0f;
	else {
		dot0 = vec3_dot(g0, vec3_new(x0, y0, z0));
		n0 = (t0*t0*t0*t0)*dot0;
		d0.x = g0.x * (t0 * t0 * t0 * t0) - (8.0f * x0 * t0 * t0 * t0 * dot0); 
		d0.y = g0.y * (t0 * t0 * t0 * t0) - (8.0f * y0 * t0 * t0 * t0 * dot0); 
		d0.z = g0.z * (t0 * t0 * t0 * t0) - (8.0f * z0 * t0 * t0 * t0 * dot0); 
	}

	float t1 = cont - x1*x1 - y1*y1 - z1*z1;
	if (t1 < 0.0f)
		n1 = 0.0f, dot1 = 0.0f;
	else
	{
		dot1 = vec3_dot(g1, vec3_new(x1, y1, z1));
		n1 = (t1*t1*t1*t1)*dot1;
		d1.x = g1.x * (t1 * t1 * t1 * t1) - (8.0f * x1 * t1 * t1 * t1 * dot1); 
		d1.y = g1.y * (t1 * t1 * t1 * t1) - (8.0f * y1 * t1 * t1 * t1 * dot1); 
		d1.z = g1.z * (t1 * t1 * t1 * t1) - (8.0f * z1 * t1 * t1 * t1 * dot1); 
	}

	float t2 = cont - x2*x2 - y2*y2 - z2*z2;
	if (t2 < 0.0f)
		n2 = 0.0f, dot2 = 0.0f;
	else
	{
		dot2 = vec3_dot(g2, vec3_new(x2, y2, z2));
		n2 = (t2*t2*t2*t2)*dot2;
		d2.x = g2.x * (t2 * t2 * t2 * t2) - (8.0f * x2 * t2 * t2 * t2 * dot2); 
		d2.y = g2.y * (t2 * t2 * t2 * t2) - (8.0f * y2 * t2 * t2 * t2 * dot2); 
		d2.z = g2.z * (t2 * t2 * t2 * t2) - (8.0f * z2 * t2 * t2 * t2 * dot2); 

	}

	float t3 = cont - x3*x3 - y3*y3 - z3*z3;
	if (t3 < 0.0f)
		n3 = 0.0f, dot3 = 0.0f;
	else
	{
		dot3 = vec3_dot(g3, vec3_new(x3, y3, z3));
		n3 = (t3*t3*t3*t3)*dot3;
		d3.x = g3.x * (t3 * t3 * t3 * t3) - (8.0f * x3 * t3 * t3 * t3 * dot3); 
		d3.y = g3.y * (t3 * t3 * t3 * t3) - (8.0f * y3 * t3 * t3 * t3 * dot3); 
		d3.z = g3.z * (t3 * t3 * t3 * t3) - (8.0f * z3 * t3 * t3 * t3 * dot3); 
	}

	float result =  scale * (n0 + n1 + n2 + n3);
	d->x = scale * (d0.x + d1.x + d2.x + d3.x);
	d->y = scale * (d0.y + d1.y + d2.y + d3.y);
	d->z = scale * (d0.z + d1.z + d2.z + d3.z);
	return result;
}


float fbm(noise *noise, vec3 v, float h, int octaves) {
	float g = exp2(-h);
	float f = 1.0f;
	float a = 1.0f;
	float t = 0.0f;

	float scale = 0.0f;

	for (int i = 0; i < octaves; ++i) {
		vec3 vp = vec3_mul(v, f);
		t += a * noise->Noise(noise->source, &vp);
		f *= 2.0f;

		scale += a;
		a *= g;
	}
	return t / scale;
}

float fbm_with_derivative(noise *noise, vec3 v, float h, int octaves, vec3 *d) {
	vec3 derivative = vec3_new(0.0f, 0.0f, 0.0f);
	float g = exp2(-h);
	float f = 1.0f;
	float a = 1.0f;
	float t = 0.0f;

	float scale = 0.0f;

	for (int i = 0; i < octaves; ++i) {
		vec3 vp = vec3_mul(v, f);

		t += a * noise->NoiseD(noise->source, &vp, &derivative);
		d->x += a * derivative.x;
		d->y += a * derivative.y;
		d->z += a * derivative.z;

		f *= 2.0f;
		scale += a;
		a *= g;
	}
	//*d = derivative;
	d->x /= scale;
	d->y /= scale;
	d->z /= scale;
	return t / scale;
}
