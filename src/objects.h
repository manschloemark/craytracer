#ifndef OBJECTS_H
#define OBJECTS_H

#include "vector3.h"
#include "ray.h"
#include "color.h"
#include "material.h"
#include "texture.h"
#include "memory.h"

typedef struct {
	point3 pt;
	vec3 n;
	material *mat;
	texture *text;
	float t_min, t_max, t;
	float u, v;
	int hit_front;
} hit_record;

// Ensure the normal is pointing in the opposite direction of the incident ray
int outward_normal(vec3 incident, vec3 *normal);

typedef struct {
	vec3 center;
	float radius;
} sphere;

sphere sphere_new(vec3 center, float radius);

typedef struct {
	point3 a, b, c;
	int double_sided;
} triangle;

triangle triangle_new(point3 x, point3 y, point3 z, int double_sided);

enum ShapeID {
	Sphere, Triangle
};

union shape {
	sphere sphere;
	triangle triangle;
};

typedef struct {
	union shape shape;
	material *mat;
	texture *text;
	enum ShapeID id;
} object;

// TODO : I only make s a pointer here because I don't feel like fixing all the ->, fix it later (also in IntersectTriangle)
int IntersectSphere(object *obj, ray *r, hit_record *hitrec);

int IntersectTriangle(object *obj, ray *r, hit_record *hitrec);

object make_sphere(point3 center, float r, texture *text, material *mat);

object make_triangle(point3 a, point3 b, point3 c, int double_sided, texture *text, material *mat);

object *add_sphere(memory_region *region, point3 center, float r, texture *text, material *mat);

object *add_triangle(memory_region *region, point3 a, point3 b, point3 c, texture *text, material *mat);
object *add_single_sided_triangle(memory_region *region, point3 a, point3 b, point3 c, texture *text, material *mat);

int Intersect(object *obj, ray *r, hit_record *hitrec);

#endif
