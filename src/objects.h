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
	point3 a, b, c;
} triangle;

triangle triangle_new(point3 x, point3 y, point3 z) {
	triangle t = {};
	t.a = x;
	t.b = y;
	t.c = z;
	return t;
}

float IntersectTriangle(triangle *tri, ray *r ){
	vec3 BA = vec3_sub(tri->b, tri->a);
	vec3 CA = vec3_sub(tri->c, tri->a);

	vec3 dir_cross_CA = vec3_cross(*r->dir, CA);
	float determinant = vec3_dot(BA, dir_cross_CA);

	// TODO : change this small value to a variable
	if (fabs(determinant) < 0.0001) return -1.0;

	float inv_determinant = 1.0 / determinant;

	vec3 dirA = vec3_sub(*r->dir, tri->a);

	float u = vec3_dot(dirA, dir_cross_CA) * inv_determinant;

	if (u < 0.0 || u > 1.0) return -1.0;

	vec3 dirA_cross_BA = vec3_cross(dirA, BA);
	float v = vec3_dot(*r->dir, dirA_cross_BA) * inv_determinant;

	if (v < 0.0 || u + v > 1.0) return -1.0;

	float t = vec3_dot(CA, dirA_cross_BA) * inv_determinant;

	// NOTE : u and v are the uv coordinates. I don't need them yet but that'll be useful.

	return t;
}

enum ShapeID {
	Sphere, Triangle
};

union shape {
	sphere sphere;
	triangle triangle;
};

typedef struct {
	union shape shape;
	fcolor color;
	enum ShapeID id;
} object;

object make_sphere(point3 center, float r, fcolor color) {
	object o;
	o.id = Sphere;
	o.shape.sphere = sphere_new(center, r);
	o.color = color;
	return o;
}

object make_triangle(point3 a, point3 b, point3 c, fcolor color) {
	object o;
	o.id = Triangle;
	o.shape.triangle = triangle_new(a, b, c);
	o.color = color;
	return o;
}
float Intersect(object *obj, ray *r) {
	switch (obj->id) {
		case Sphere:
			return IntersectSphere(&obj->shape.sphere, r);
		case Triangle:
			return IntersectTriangle(&obj->shape.triangle, r);
		default:
			return -1.0;
	}
}

vec3 Normal(object *obj, point3 intersection) {
	switch (obj->id) {
		case Sphere:
			return vec3_unit(vec3_sub(intersection, obj->shape.sphere.center)); // NOTE : IDK if I need to move this to another function or not. Seems simple as is.
		case Triangle:
			// NOTE : This is not at all correct but it's close enough for now
			return vec3_cross(obj->shape.triangle.a, obj->shape.triangle.b);
		default:
			return intersection;
	}
}

#endif
