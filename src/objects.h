#ifndef OBJECTS_H
#define OBJECTS_H

#include "common.h"
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
	double t_min, t_max, t;
	double u, v;
	int hit_front;
} hit_record;

// Ensure the normal is pointing in the opposite direction of the incident ray
int outward_normal(vec3 incident, vec3 *normal);

typedef struct {
	vec3 center;
	double radius;
} sphere;

sphere sphere_new(vec3 center, double radius);

typedef struct {
	point3 a, b, c;
	int double_sided;
} triangle;

triangle triangle_new(point3 x, point3 y, point3 z, int double_sided);

typedef struct {
	void *tri_a, *tri_b;
} quad;

quad quad_new(memory_region *region, point3 a, point3 b, point3 c, point3 d, int double_sided, texture *text, material *mat);

typedef struct {
	noise *noise;
	void *obj;
	double hurst;
	double offset_scale, scale;
	double _epsilon; // NOTE : this is only used for "stringy fbm shapes" (please excuse the name)
	int octaves;

} fbm_shape;

fbm_shape fbm_shape_new(memory_region *region, noise *noise, double scale, double offset_scale, double hurst, int octaves, void *obj);

enum ShapeID {
	Sphere, Triangle, Quad, FBMShape,
		SHAPE_ERROR
};

union shape {
	sphere sphere;
	triangle triangle;
	quad quad;
	fbm_shape fbm_shape;
};

typedef struct {
	int (*Intersect)(void *self, ray *r, hit_record *hitrec, thread_context *thread);
	vec3 (*Normal)(void *self, vec3 relative);
	union shape shape;
	material *mat;
	texture *text;
	enum ShapeID id;
} object;

object make_sphere(point3 center, double r, texture *text, material *mat);

object make_triangle(point3 a, point3 b, point3 c, int double_sided, texture *text, material *mat);

object make_quad(memory_region *region, vec3 a, vec3 b, vec3 c, vec3 d, int double_sided, texture *text, material *mat);

object make_fbm_shape(memory_region *region, noise *noise, double scale, double offset_scale, double hurst, int octabes, object *obj);
object make_fbm_sphere(memory_region *region, noise *noise, double scale, double offset_scale, double hurst, int octaves, object *obj);

object *add_sphere(memory_region *region, point3 center, double r, texture *text, material *mat);

object *add_triangle(memory_region *region, point3 a, point3 b, point3 c, texture *text, material *mat);
object *add_single_sided_triangle(memory_region *region, point3 a, point3 b, point3 c, texture *text, material *mat);

object *add_quad(memory_region *region, vec3 a, vec3 b, vec3 c, vec3 d, texture *text, material *mat);
object *add_ss_quad(memory_region *region, vec3 a, vec3 b, vec3 c, vec3 d, texture *text, material *mat);

object *add_fbm_shape(memory_region *region, noise *noise, double scale, double offset_scale, double hurst, int octaves, object *obj);
object *add_fbm_sphere(memory_region *region, noise *noise, double scale, double offset_scale, double hurst, int octaves, object *obj);

object *add_stringy_sphere(memory_region *region, noise *noise, double epsilon, double scale, double offset_scale, double hurst, int octaves, object *obj);


// TODO : I only make s a pointer here because I don't feel like fixing all the ->, fix it later (also in IntersectTriangle)
int IntersectSphere(void *self, ray *r, hit_record *hitrec, thread_context *thread);

int IntersectTriangle(void *self, ray *r, hit_record *hitrec, thread_context *thread);
int IntersectQuad(void *self, ray *r, hit_record *hitrec, thread_context *thread);

int Intersect(void *self, ray *r, hit_record *hitrec, thread_context *thread);

int IntersectFBMShape(void *self, ray *r, hit_record *hitrec, thread_context *thread);
int IntersectFBMSphere(void *self, ray *r, hit_record *hitrec, thread_context *thread);
int IntersectStringySphere(void *self, ray *r, hit_record *hitrec, thread_context *thread);

vec3 SphereNormal(void *self, vec3 relative);

vec3 TriangleNormal(void *self, vec3 relative);

vec3 FBMNormal(void *self, vec3 relative);

#endif
