#ifndef VECTOR3_H
#define VECTOR3_H

#include <math.h>

struct Vector3 {
	float x, y, z;
};
typedef struct Vector3 vec3;

static inline vec3 vec3_new(float x, float y, float z) {
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

vec3 vec3_sub(vec3 a, vec3 b) {
	vec3 v = {a.x-b.x, a.y-b.y, a.z-b.z};
	return v;
}

float vec3_dot(vec3 a, vec3 b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

vec3 vec3_cross(vec3 a, vec3 b) {
	vec3 v;
	v.x = a.y*b.z - a.z*b.y;   // i
	v.y = (a.z*b.x - a.x*b.z); // -j
	v.z = a.x*b.y - a.y*b.x;   // k
	return v;
}

// Vector/Scalar operations
vec3 vec3_mul(vec3 a, float s) {
	vec3 v = {a.x*s, a.y*s, a.z*s};
	return v;
}

vec3 vec3_div(vec3 a, float s) {
	vec3 v = {a.x/s, a.y/s, a.z/s};
	return v;
}

// Vector functions
float vec3_lensq(vec3 a) {
	return a.x*a.x + a.y*a.y + a.z*a.z;
}

float vec3_len(vec3 a) {
	return sqrtf(vec3_lensq(a));
}

vec3 vec3_neg(vec3 a) {
	vec3 v = {-a.x, -a.y, -a.z};
	return v;
}

vec3 vec3_abs(vec3 a) {
	vec3 v = {fabsf(a.x), fabsf(a.y), fabsf(a.z)};
	return v;
}

vec3 vec3_unit(vec3 a) {
	return vec3_div(a, vec3_len(a));
}
#endif
