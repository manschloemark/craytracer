#ifndef PERLIN_H
#define PERLIN_H

#include "common.h"
#include "memory.h"

#define POINTCOUNT8 8

typedef struct {
	vec3 *random_vectors;
	int *perm_x, *perm_y, *perm_z;
	int pointcount;
} perlin;

void permute(int *perm, int pointcount, thread_context *thread);

perlin perlin_new(int pointcount, thread_context *thread);
perlin *add_perlin(memory_region *region, int pointcount, thread_context *thread);

float perlin_interpolate(vec3 c[2][2][2], float u, float v, float w);
float perlin_noise(void *perl, point3 *pt);
float perlin_turbulence(perlin *perl, point3 *pt, int depth);

float perlin_interpolate_with_derivative(vec3 c[2][2][2], float u, float v, float w, vec3 *d);
float perlin_noise_with_derivative(void *perl, point3 *pt, vec3 *d);


// TODO : implement simplex noise
typedef struct {
	vec3 *gradients;
	int *perm_x, *perm_y, *perm_z;
	float skew, unskew;
	int pointcount;
} simplex;

simplex simplex_new(int pointcount, thread_context *thread);
simplex *add_simplex(memory_region *region, int pointcount, thread_context *thread);

float simplex_noise(void *simp, vec3 *pt);
float simplex_noise_with_derivative(void *simp, vec3 *pt, vec3 *d);

typedef struct {
	float (*Noise) (void *source, point3 *pt);
	float (*NoiseD) (void *source, point3 *pt, vec3 *d);
	void *source;
} noise;

noise *add_perlin_noise(memory_region *region, int pointcount, thread_context *thread);
noise *add_simplex_noise(memory_region *region, int pointcount, thread_context *thread);

float fbm(noise *noise, vec3 v, float h, int octaves);
float fbm_with_derivative(noise *noise, vec3 v, float h, int octaves, vec3 *d);

#endif
