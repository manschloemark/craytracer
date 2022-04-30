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


float fbm(perlin *perl, vec3 v, float h, int octaves);

#endif
