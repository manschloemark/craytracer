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

double perlin_interpolate(vec3 c[2][2][2], double u, double v, double w);
double perlin_noise(void *perl, point3 *pt);
double perlin_turbulence(perlin *perl, point3 *pt, int depth);

double perlin_interpolate_with_derivative(vec3 c[2][2][2], double u, double v, double w, vec3 *d);
double perlin_noise_with_derivative(void *perl, point3 *pt, vec3 *d);


// TODO : implement simplex noise
typedef struct {
	vec3 *gradients;
	int *perm_x, *perm_y, *perm_z;
	double skew, unskew;
	int pointcount;
} simplex;

simplex simplex_new(int pointcount, thread_context *thread);
simplex *add_simplex(memory_region *region, int pointcount, thread_context *thread);

double simplex_noise(void *simp, vec3 *pt);
double simplex_noise_with_derivative(void *simp, vec3 *pt, vec3 *d);

typedef struct {
	double (*Noise) (void *source, point3 *pt);
	double (*NoiseD) (void *source, point3 *pt, vec3 *d);
	void *source;
} noise;

noise *add_perlin_noise(memory_region *region, int pointcount, thread_context *thread);
noise *add_simplex_noise(memory_region *region, int pointcount, thread_context *thread);

double fbm(noise *noise, vec3 v, double h, int octaves);
double fbm_with_derivative(noise *noise, vec3 v, double h, int octaves, vec3 *d);

#endif
