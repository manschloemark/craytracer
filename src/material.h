#ifndef MATERIAL_H
#define MATERIAL_H

#include "vector3.h"
#include "ray.h"
#include "memory.h"

vec3 vec3_reflect(vec3 v, vec3 n);
vec3 vec3_refract(vec3 v, vec3 n, float ior_ratio);

typedef struct {
} lambertian;

vec3 ScatterLambertian(vec3 *n);

typedef struct { 
	float blur;
} metal;

static inline metal metal_new(float blur) {
	metal metal = {};
	metal.blur = blur;
	return metal;
}

vec3 ScatterMetal(metal m, vec3 v, vec3 *n);

// TODO : find a better name for this. It's not just glass, but any medium light can travel _through_
typedef struct {
	float ior;
} glass;

static inline glass glass_new(float ior) {
	glass glass = {};
	glass.ior = ior;
	return glass;
}

// Schlick's Approximation is a formula for approximating contribution of Fresnel factor in specular reflection
// 1.0 here is tecnically n1 but it is assumed to be air all the time.
float schlick_approximation(float cos_incident, float ior_ratio);

vec3 ScatterGlass(glass g, vec3 v, vec3 *n, float n1, int hit_front);

enum MaterialID {
	Lambertian,
	Metal,
	Glass,
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
	enum MaterialID id;
	int has_color;
} material;

material make_lambertian();

material make_metal(float blur); 

material make_glass(float ior); 

material *add_lambertian(memory_region *region); 

material *add_metal(memory_region *region, float blur); 

material *add_glass(memory_region *region, float ior); 

int Scatter(material *mat, int hit_front, vec3 *n, ray *r); 
#endif
