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

typedef struct {
	perlin *perlin;
	void *obj;
	float hurst, scale;
	int octaves;
} fbm_shape;

fbm_shape fbm_shape_new(memory_region *region, float hurst, int octaves, void *obj);

enum ShapeID {
	Sphere, Triangle, FBMShape
};

union shape {
	sphere sphere;
	triangle triangle;
	fbm_shape fbm_shape;
};

typedef struct {
	int (*Intersect)(void *self, ray *r, hit_record *hitrec);
	vec3 (*Normal)(void *self, vec3 relative);
	union shape shape;
	material *mat;
	texture *text;
	enum ShapeID id;
} object;

object make_sphere(point3 center, float r, texture *text, material *mat);

object make_triangle(point3 a, point3 b, point3 c, int double_sided, texture *text, material *mat);

object make_fbm_shape(memory_region *region, float hurst, int octabes, object *obj);
object make_fbm_sphere(memory_region *region, float hurst, int octabes, object *obj);

object *add_sphere(memory_region *region, point3 center, float r, texture *text, material *mat);

object *add_triangle(memory_region *region, point3 a, point3 b, point3 c, texture *text, material *mat);
object *add_single_sided_triangle(memory_region *region, point3 a, point3 b, point3 c, texture *text, material *mat);

object *add_fbm_shape(memory_region *region, float hurst, int octaves, object *obj);
object *add_fbm_sphere(memory_region *region, float hurst, int octaves, object *obj);


// TODO : I only make s a pointer here because I don't feel like fixing all the ->, fix it later (also in IntersectTriangle)
int IntersectSphere(void *self, ray *r, hit_record *hitrec);

int IntersectTriangle(void *self, ray *r, hit_record *hitrec);

int Intersect(void *self, ray *r, hit_record *hitrec);

int IntersectFBMShape(void *self, ray *r, hit_record *hitrec);
int IntersectFBMSphere(void *self, ray *r, hit_record *hitrec);

vec3 SphereNormal(void *self, vec3 relative);

vec3 TriangleNormal(void *self, vec3 relative);

vec3 FBMNormal(void *self, vec3 relative);

#endif
