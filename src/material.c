#include <math.h>
#include "material.h"

vec3 vec3_reflect(vec3 v, vec3 n) {
	return vec3_sub(v, vec3_mul(n, 2.0 * vec3_dot(v, n)));
}

vec3 vec3_refract(vec3 v, vec3 n, float ior_ratio) {
	float cos_theta = vec3_dot(vec3_neg(v), n);
	if (cos_theta > 1.0) cos_theta = 1.0;
	vec3 horizontal_component = vec3_mul(vec3_add(v, vec3_mul(n, cos_theta)), ior_ratio);
	vec3 vertical_component = vec3_mul(n, -1.0 * sqrtf(fabs(1.0 - vec3_lensq(horizontal_component))));
	return vec3_add(horizontal_component, vertical_component);
}

vec3 ScatterLambertian(vec3 *n) {
	vec3 v = vec3_random_in_hemisphere(*n);
	if (vec3_near_zero(v)) return *n;
	return v;
}

vec3 ScatterMetal(metal m, vec3 v, vec3 *n) {
	//return vec3_reflect(v, *n);
	return vec3_add(vec3_reflect(v, *n), vec3_mul(vec3_random(), m.blur));
}

// Schlick's Approximation is a formula for approximating contribution of Fresnel factor in specular reflection
// 1.0 here is tecnically n1 but it is assumed to be air all the time.
float schlick_approximation(float cos_incident, float ior_ratio) {
	float R0 = (1.0 - ior_ratio) / (1.0 + ior_ratio);
	R0 = R0 * R0;
	return R0 + (1.0 - R0) * pow(1.0 - cos_incident, 5.0);
}

// NOTE : This assumes you are leaving / entering air with ior=1.0.
vec3 ScatterGlass(glass g, vec3 v, vec3 *n, float n1, int hit_front) {
	float ior_ratio = (hit_front) ? n1 / g.ior : g.ior / n1;
	vec3 unit_dir = vec3_unit(v);
	float cos_theta = vec3_dot(vec3_neg(unit_dir), *n);
	if (cos_theta > 1.0) cos_theta = 1.0;
	float sin_theta = sqrtf(1.0 - cos_theta * cos_theta);

	// NOTE : should I do something different if it's at the critical angle?
	vec3 scattered_dir;
	int past_critical_angle = (sin_theta * ior_ratio) > 1.0;
	if (past_critical_angle || schlick_approximation(cos_theta, ior_ratio) > random_float()) {
		scattered_dir = vec3_reflect(unit_dir, *n); 
	} else {
		scattered_dir = vec3_refract(unit_dir, *n, ior_ratio);
	}
	return scattered_dir;
}

material make_lambertian() {
	material mat = {};
	mat.id = Lambertian;
	mat.has_color = 1;
	return mat;
}

material make_metal(float blur) {
	material mat = {};
	mat.id = Metal;
	mat.has_color = 1;
	mat.surface.metal = metal_new(blur);
	return mat;
}

material make_glass(float ior) {
	material mat = {};
	mat.id = Glass;
	mat.has_color = 1;
	mat.surface.glass = glass_new(ior);
	return mat;
}

material *add_lambertian(memory_region *region) {
	material mat = make_lambertian();
	return (material *)memory_region_add(region, &mat, sizeof(material));
}

material *add_metal(memory_region *region, float blur) {
	material mat = make_metal(blur);
	return (material *)memory_region_add(region, &mat, sizeof(material));
}

material *add_glass(memory_region *region, float ior) {
	material mat = make_glass(ior);
	return (material *)memory_region_add(region, &mat, sizeof(material));
}

// 1 = Scattered ray, 0 = no scatter
int Scatter(material *mat, int hit_front, vec3 *n, ray *r) {
	switch (mat->id) {
		case Lambertian:
			r->dir = ScatterLambertian(n);
			return 1;
		case Metal:
			r->dir = ScatterMetal(mat->surface.metal, r->dir, n);
			return 1;
		case Glass:
			r->dir = ScatterGlass(mat->surface.glass, r->dir, n, 1.0, hit_front);
			return 1;
		default:
			return 0;
	}
}
/*
int MatColor(material *mat, fcolor *color) {
	switch (mat->id) {
		case Diffuse:
			*color = mat->color;
			return 1;
		default:
			return 0;
	}
}
*/
