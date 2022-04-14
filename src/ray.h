#ifndef RAY_H
#define RAY_H

#include "vector3.h"

// TODO : Maybe I should change ray members away from pointers

struct Ray {
	point3 pt;
	vec3 dir;
};
typedef struct Ray ray;

ray ray_new(point3 pt, vec3 dir) {
	ray r = {};
	r.pt = pt;
	r.dir = dir;
	return r;
}

point3 pt_on_ray(ray *r, float t) {
	return vec3_add(r->pt, vec3_mul(r->dir, t));
}


#endif
