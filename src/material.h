#ifndef MATERIAL_H
#define MATERIAL_H

#include "vector3.h"
#include "color.h"
#include "objects.h"

#include "memory.h"

// TODO : maybe decouple color from material that way I don't need two materials just for a different color object

typedef struct {
} lambertian;

vec3 ScatterLambertian(vec3 *n) {
	vec3 v = vec3_random_in_hemisphere(*n);
	if (vec3_near_zero(v)) return *n;
	return v;
}

typedef struct { 
	float blur;
} metal;

metal metal_new(float blur) {
	metal metal = {};
	metal.blur = blur;
	return metal;
}

vec3 vec3_reflect(vec3 v, vec3 n) {
	return vec3_sub(v, vec3_mul(n, 2.0 * vec3_dot(v, n)));
}

vec3 ScatterMetal(metal m, vec3 v, vec3 *n) {
	//return vec3_reflect(v, *n);
	return vec3_add(vec3_reflect(v, *n), vec3_mul(vec3_random(), m.blur));
}

enum MaterialID {
	Lambertian,
	Metal,
};

// NOTE : I wanted this to be analgous to the union shape from objects.hit
// but right now I can't of how to name this. I'll go with surface for now.
union surface {
	lambertian lambertian;
	metal metal;
};

typedef struct {
	union surface surface;
	enum MaterialID id;
	int has_color;
} material;

material make_lambertian() {
	material mat = {};
	mat.id = Lambertian;
	mat.has_color = 1;
	return mat;
}

material make_metal(float blur) {
	material mat = {};
	mat.id = Metal;
	mat.has_color = 1;
	mat.surface.metal = metal_new(blur);
	return mat;
}

material *add_lambertian(memory_region *region) {
	material mat = make_lambertian();
	return (material *)memory_region_add(region, &mat, sizeof(material));
}

material *add_metal(memory_region *region, float blur) {
	material mat = make_metal(blur);
	return (material *)memory_region_add(region, &mat, sizeof(material));
}

// 1 = Scattered ray, 0 = no scatter
int Scatter(material *mat, vec3 *n, ray *r) {
	switch (mat->id) {
		case Lambertian:
			r->dir = ScatterLambertian(n);
			return 1;
		case Metal:
			r->dir = ScatterMetal(mat->surface.metal, r->dir, n);
			return 1;
		default:
			return 0;
	}
}
/*
int MatColor(material *mat, fcolor *color) {
	switch (mat->id) {
		case Diffuse:
			*color = mat->color;
			return 1;
		default:
			return 0;
	}
}
*/

#endif
