#ifndef VECTOR3_H
#define VECTOR3_H

#include <math.h>
#include "common.h"

struct Vector3 {
	float x, y, z;
};
typedef struct Vector3 vec3;
typedef struct Vector3 point3;

vec3 vec3_new(float x, float y, float z);

// Vector/Vector operations
vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_addf(vec3 a, float f);

static inline vec3 vec3_sub(vec3 a, vec3 b) {
	vec3 v = {a.x-b.x, a.y-b.y, a.z-b.z};
	return v;
}

static inline float vec3_dot(vec3 a, vec3 b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

static inline vec3 vec3_cross(vec3 a, vec3 b) {
	vec3 v;
	v.x = a.y*b.z - a.z*b.y;   // i
	v.y = (a.z*b.x - a.x*b.z); // -j
	v.z = a.x*b.y - a.y*b.x;   // k
	return v;
}

// Vector/Scalar operations
static inline vec3 vec3_mul(vec3 a, float s) {
	vec3 v = {a.x*s, a.y*s, a.z*s};
	return v;
}

static inline vec3 vec3_div(vec3 a, float s) {
	vec3 v = {a.x/s, a.y/s, a.z/s};
	return v;
}

// Vector functions
static inline float vec3_lensq(vec3 a) {
	return a.x*a.x + a.y*a.y + a.z*a.z;
}

float vec3_len(vec3 a);

static inline vec3 vec3_neg(vec3 a) {
	vec3 v = {-a.x, -a.y, -a.z};
	return v;
}

vec3 vec3_abs(vec3 a);

static inline int vec3_near_zero(vec3 v) {
	const float epsilon = 0.000001;
	vec3 abs_v = vec3_abs(v);
	return (abs_v.x < epsilon) && (abs_v.y < epsilon) && (abs_v.z < epsilon);
}

vec3 vec3_unit(vec3 a);

vec3 vec3_random();

vec3 vec3_bounded_random(float min, float max);

vec3 vec3_random_in_unit_sphere();

vec3 vec3_random_in_hemisphere(vec3 n);

#endif
