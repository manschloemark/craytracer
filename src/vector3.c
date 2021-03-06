#include "vector3.h"

vec3 vec3_new(double x, double y, double z) {
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

vec3 vec3_addf(vec3 a, double f) {
	vec3 v = {a.x+f, a.y+f, a.z+f};
	return v;
}

double vec3_len(vec3 a) {
	return sqrt(vec3_lensq(a));
}

vec3 vec3_abs(vec3 a) {
	vec3 v = {fabs(a.x), fabs(a.y), fabs(a.z)};
	return v;
}

vec3 vec3_unit(vec3 a) {
	double len = vec3_len(a);
	if (len) return vec3_div(a, vec3_len(a));
	return a;
}

vec3 vec3_random(struct drand48_data *state) {
	vec3 v = {};
	v.x = random_double(state);
	v.y = random_double(state);
	v.z = random_double(state);
	return v;
}

vec3 vec3_bounded_random(double min, double max, struct drand48_data *state) {
	vec3 v = {};
	v.x = random_double_between(min, max, state);
	v.y = random_double_between(min, max, state);
	v.z = random_double_between(min, max, state);
	return v;
}

// Pretty much copied from Raytracing in One Weekend.
vec3 vec3_random_in_unit_sphere(struct drand48_data *state) {
	while(1) {
		vec3 v = vec3_bounded_random(-1.0, 1.0, state);
		if (vec3_lensq(v) < 1.0) return v;
	}
}

// According to Raytracing in One Weekend this provides a better distribution
// for lambertian surfaces
vec3 vec3_random_in_hemisphere(vec3 n, struct drand48_data *state) {
	vec3 v = vec3_random_in_unit_sphere(state);
	if (vec3_dot(v, n) > 0.0) return v;
	else return vec3_neg(v);

}
