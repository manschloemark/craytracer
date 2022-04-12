#ifndef OBJECTS_H
#define OBJECTS_H


#include "vector3.h"
#include "ray.h"
#include "color.h"

struct hit_record {
	point3 pt;
	vec3 n;
	//material *mat;
	float t_min, t_max, t;
};

// Ensure the normal is pointing in the opposite direction of the incident ray
void outward_normal(vec3 *incident, vec3 *normal) {
	if(vec3_dot(*incident, *normal) > 0.0) *normal = vec3_neg(*normal);
}

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

int IntersectSphere(sphere *s, ray *r, struct hit_record *hitrec) {
	vec3 T = vec3_sub(*r->pt, s->center);
	float a = vec3_lensq(*r->dir);
	float half_b = vec3_dot(T, *r->dir);
	float c = vec3_lensq(T) - s->radius*s->radius; 

	float discriminant = half_b*half_b - a*c;

	if (discriminant < 0.0) {
		return 0;
	}

	float root = (-half_b - sqrt(discriminant)) / a;
	if (root < hitrec->t_min || root > hitrec->t) {
		root = (- half_b + sqrt(discriminant)) / a;
		if (root < hitrec->t_min || root > hitrec->t) return 0;
	}

	hitrec->t = root;
	hitrec->pt = pt_on_ray(r, root);
	vec3 n = vec3_unit(vec3_sub(hitrec->pt, s->center));
	outward_normal(r->dir, &n); // NOTE : IDK if I need to move this to another function or not. Seems simple as is.
	hitrec->n = n;
	return 1;
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

int IntersectTriangle(triangle *tri, ray *r, struct hit_record *hitrec){
	vec3 BA = vec3_sub(tri->b, tri->a);
	vec3 CA = vec3_sub(tri->c, tri->a);

	vec3 dir_cross_CA = vec3_cross(*r->dir, CA);
	float determinant = vec3_dot(BA, dir_cross_CA);

	// TODO : change this small value to a variable
	if (fabs(determinant) < 0.0001) return 0;

	float inv_determinant = 1.0 / determinant;

	vec3 dirA = vec3_sub(*r->dir, tri->a);

	float u = vec3_dot(dirA, dir_cross_CA) * inv_determinant;

	if (u < 0.0 || u > 1.0) return 0;

	vec3 dirA_cross_BA = vec3_cross(dirA, BA);
	float v = vec3_dot(*r->dir, dirA_cross_BA) * inv_determinant;

	if (v < 0.0 || u + v > 1.0) return 0;

	float t = vec3_dot(CA, dirA_cross_BA) * inv_determinant;

	if (t < hitrec->t_min || t > hitrec->t) return 0;

	hitrec->t = t;
	hitrec->pt = pt_on_ray(r, t);
	vec3 n = vec3_cross(BA, CA);
	outward_normal(r->dir, &n);
	hitrec->n = n;

	return 1;
	// NOTE : u and v are the uv coordinates. I don't need them yet but that'll be useful.
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

int Intersect(object *obj, ray *r, struct hit_record *hitrec) {
	switch (obj->id) {
		case Sphere:
			return IntersectSphere(&obj->shape.sphere, r, hitrec);
		case Triangle:
			return IntersectTriangle(&obj->shape.triangle, r, hitrec);
		default:
			return 0;
	}
}

#endif
