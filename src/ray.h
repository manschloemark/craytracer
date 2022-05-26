#ifndef RAY_H
#define RAY_H

#include "vector3.h"

// TODO : Maybe I should change ray members away from pointers

struct Ray {
	point3 pt;
	vec3 dir;
};
typedef struct Ray ray;

static inline ray ray_new(point3 pt, vec3 dir) {
	ray r = {pt, dir};
	return r;
}

static inline point3 pt_on_ray(ray *r, double t) {
	return vec3_add(r->pt, vec3_mul(r->dir, t));
}


#endif
