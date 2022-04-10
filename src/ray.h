#ifndef RAY_H
#define RAY_H

#include "vector3.h"

struct Ray {
	point3 *pt;
	vec3 *dir;
};
typedef struct Ray ray;

point3 pt_on_ray(ray *r, float t) {
	return vec3_add(*r->pt, vec3_mul(*r->dir, t));
}


#endif
