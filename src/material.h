#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.h"
#include "vector3.h"
#include "ray.h"
#include "memory.h"

vec3 vec3_reflect(vec3 v, vec3 n);
vec3 vec3_refract(vec3 v, vec3 n, double ior_ratio);

typedef struct {
} lambertian;


typedef struct { 
	double blur;
} metal;

static inline metal metal_new(double blur) {
	metal metal = {};
	metal.blur = blur;
	return metal;
}


// TODO : find a better name for this. It's not just glass, but any medium light can travel _through_
typedef struct {
	double ior;
} glass;

static inline glass glass_new(double ior) {
	glass glass = {};
	glass.ior = ior;
	return glass;
}

// This feels kind of pointless, like the lambertian struct. But for posterity I guess I'll do this
typedef struct {
} diffuse_light;

// Schlick's Approximation is a formula for approximating contribution of Fresnel factor in specular reflection
// 1.0 here is tecnically n1 but it is assumed to be air all the time.
double schlick_approximation(double cos_incident, double ior_ratio);


enum MaterialID {
	Lambertian,
	Metal,
	Glass,
	DiffuseLight,
};

// NOTE : I wanted this to be analgous to the union shape from objects.hit
// but right now I can't of how to name this. I'll go with surface for now.
union surface {
	lambertian lambertian;
	metal metal;
	glass glass;
};

typedef struct {
	union surface surface;
	int (*Scatter)(void *self, int hit_front, vec3 *n, ray *r, thread_context *thread);
	enum MaterialID id;
	int has_color;
} material;

material make_lambertian();

material make_metal(double blur); 

material make_glass(double ior); 

material make_diffuse_light(); 

material *add_lambertian(memory_region *region); 

material *add_metal(memory_region *region, double blur); 

material *add_glass(memory_region *region, double ior); 

material *add_diffuse_light(memory_region *region); 

int ScatterLambertian(void *self, int hit_front, vec3 *n, ray *r, thread_context *thread); 
int ScatterMetal(void *self, int hit_front, vec3 *n, ray *r, thread_context *thread); 
int ScatterGlass(void *self, int hit_front, vec3 *n, ray *r, thread_context *thread); 
int ScatterDiffuseLight(void *self, int hit_front, vec3 *n, ray *r, thread_context *thread); 

#endif
