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
	float a = vec3_lensq(*r->dir);
	float half_b = vec3_dot(T, *r->dir);
	float c = vec3_lensq(T) - s->radius*s->radius; 

	float discriminant = half_b*half_b - a*c;

	if (discriminant < 0.0) {
		return -1.0;
	}

	return (-half_b - sqrt(discriminant)) / a;

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

union shape {
	sphere sphere;
	triangle triangle;
};

typedef struct {
	enum ShapeID id;
	fcolor color;
	union shape shape;
} object;

object make_sphere(point3 center, float r, fcolor color) {
	object o;
	o.id = Sphere;
	o.shape.sphere = sphere_new(center, r);
	o.color = color;
	return o;
}

float Intersect(object *obj, ray *r) {
	switch (obj->id) {
		case Sphere:
			return IntersectSphere(&obj->shape.sphere, r);
			break;
		case Triangle:
		default:
			return -1.0;
	}
}

vec3 Normal(object *obj, point3 intersection) {
	switch (obj->id) {
		case Sphere:
			return vec3_unit(vec3_sub(intersection, obj->shape.sphere.center)); // NOTE : IDK if I need to move this to another function or not. Seems simple as is.
			break;
		case Triangle:
		default:
			return intersection;
	}
}

#endif
