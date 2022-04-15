#ifndef MATERIAL_H
#define MATERIAL_H

#include "vector3.h"
#include "color.h"
#include "objects.h"

// TODO : maybe decouple color from material that way I don't need two materials just for a different color object

typedef struct {
} lambertian;

enum MaterialID {
	Lambertian,
	Metal,
};

// NOTE : I wanted this to be analgous to the union shape from objects.hit
// but right now I can't of how to name this. I'll go with surface for now.
union surface {
	lambertian lambertian;
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

// 1 = Scattered ray, 0 = no scatter
int Scatter(material *mat, vec3 *pt, vec3 *n, vec3 *reflection) {
	switch (mat->id) {
		case Lambertian:
			//*reflection = vec3_add(*n, vec3_add(*pt, vec3_random_in_unit_sphere()));
			*reflection = vec3_add(*pt, vec3_random_in_hemisphere(*n));
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
