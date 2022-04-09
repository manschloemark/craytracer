#ifndef OBJECTS_H
#define OBJECTS_H


#include "vector3.h"
#include "ray.h"
#include "color.h"

typedef struct {
	vec3 center;
	float radius;
} sphere;

sphere sphere_new(vec3 center, float radius) {
	sphere s = {};
	s.center = center;
	s.radius = radius;
	return s;
}

float IntersectSphere(sphere *s, ray *r) {
	vec3 T = vec3_sub(*r->pt, s->center);
	float a = vec3_dot(*r->dir, *r->dir);
	float b = 2.0 * vec3_dot(T, *r->dir);
	float c = vec3_dot(T, T) - s->radius*s->radius; 

	float discriminant = b*b - 4.0*a*c;

	return discriminant;
}

typedef struct {
	point3 x, y, z;
} triangle;

enum ShapeID {
	Sphere, Triangle
};

union Shape {
	sphere *sphere;
	triangle *triangle;
};

typedef struct {
	enum ShapeID id;
	union {
		sphere sphere;
		triangle triangle;
	};
	fcolor color;
} object;

object make_sphere(point3 center, float r, fcolor color) {
	object o;
	o.id = Sphere;
	o.sphere = sphere_new(center, r);
	o.color = color;
	return o;
}

float Intersect(object *obj, ray *r) {
	switch (obj->id) {
		case Sphere:
			return IntersectSphere(&obj->sphere, r);
			break;
		case Triangle:
		default:
			return -1.0;
	}
}

#endif
