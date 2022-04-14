#ifndef MATERIAL_H
#define MATERIAL_H

#include "vector3.h"
#include "color.h"
#include "objects.h"

// TODO : maybe decouple color from material that way I don't need two materials just for a different color object

typedef struct {
} diffuse;

enum MaterialID {
	Diffuse,
	Metal,
	Matte
};

// NOTE : I wanted this to be analgous to the union shape from objects.hit
// but right now I can't of how to name this. I'll go with surface for now.
union surface {
	diffuse diffuse;
};

typedef struct {
	union surface surface;
	enum MaterialID id;
	int has_color;
} material;

material make_diffuse() {
	material mat = {};
	mat.id = Diffuse;
	mat.has_color = 1;
	return mat;
}

// 1 = Scattered ray, 0 = no scatter
int Scatter(material *mat, vec3 *pt, vec3 *n, vec3 *reflection) {
	switch (mat->id) {
		case Diffuse:
			*reflection = vec3_add(*n, vec3_add(*pt, vec3_random_in_unit_sphere()));
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
