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

void perlin_gen_perm(int *perm, int pointcount);

perlin perlin_new(int pointcount);
perlin *add_perlin(memory_region *region, int pointcount);

float perlin_interpolate(vec3 c[2][2][2], float u, float v, float w);
float perlin_noise(perlin *perl, point3 *pt);
float perlin_turbulence(perlin *perl, point3 *pt, int depth);

float perlin_interpolate_with_derivative(vec3 c[2][2][2], float u, float v, float w, vec3 *d);
float perlin_noise_with_derivative(perlin *perl, point3 *pt, vec3 *d);



// TODO : implement simplex noise
typedef struct {
} simplex;

union noise_process {
	perlin perlin;
	simplex simplex;
};


typedef struct {
	float (*Noise) (void *noise, point3 *pt);
	union noise_process source;
} noise;

float fbm(perlin *perlin, vec3 v, float h, int octaves);
float fbm_with_derivative(perlin *perlin, vec3 v, float h, int octaves, vec3 *d);

#endif
