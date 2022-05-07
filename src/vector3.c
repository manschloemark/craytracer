#include "vector3.h"

vec3 vec3_new(float x, float y, float z) {
	vec3 v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

// Vector/Vector operations
vec3 vec3_add(vec3 a, vec3 b) {
	vec3 v = {a.x+b.x, a.y+b.y, a.z+b.z};
	return v;
}

vec3 vec3_addf(vec3 a, float f) {
	vec3 v = {a.x+f, a.y+f, a.z+f};
	return v;
}

float vec3_len(vec3 a) {
	return sqrtf(vec3_lensq(a));
}

vec3 vec3_abs(vec3 a) {
	vec3 v = {fabsf(a.x), fabsf(a.y), fabsf(a.z)};
	return v;
}

vec3 vec3_unit(vec3 a) {
	return vec3_div(a, vec3_len(a));
}

vec3 vec3_random() {
	vec3 v = {};
	v.x = random_float();
	v.y = random_float();
	v.z = random_float();
	return v;
}

vec3 vec3_bounded_random(float min, float max) {
	vec3 v = {};
	v.x = random_float_between(min, max);
	v.y = random_float_between(min, max);
	v.z = random_float_between(min, max);
	return v;
}

// Pretty much copied from Raytracing in One Weekend.
vec3 vec3_random_in_unit_sphere() {
	while(1) {
		vec3 v = vec3_bounded_random(-1.0, 1.0);
		if (vec3_lensq(v) < 1.0) return v;
	}
}

// According to Raytracing in One Weekend this provides a better distribution
// for lambertian surfaces
vec3 vec3_random_in_hemisphere(vec3 n) {
	vec3 v = vec3_random_in_unit_sphere();
	if (vec3_dot(v, n) > 0.0) return v;
	else return vec3_neg(v);

}
