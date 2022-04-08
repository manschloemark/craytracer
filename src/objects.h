#ifndef OBJECTS_H
#define OBJECTS_H

#endif

#include "vector3.h"
#include "ray.h"

struct Sphere {
	vec3 center;
	float radius;
};

typedef struct Sphere sphere;

float IntersectSphere(sphere *s, ray *r) {
	vec3 T = vec3_sub(*r->pt, s->center);
	float a = vec3_dot(*r->dir, *r->dir);
	float b = 2.0 * vec3_dot(T, *r->dir);
	float c = vec3_dot(T, T) - s->radius*s->radius; 

	float discriminant = b*b - 4.0*a*c;

	return discriminant;
}

