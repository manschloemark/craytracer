#ifndef COMMON_H
#define COMMON_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
	int id;
	unsigned int rand_state;
} thread_context;

static const float pi = 3.141592f;

static inline float clamp(float n, float min, float max) {
	return (n < min) ? min : (n > max) ? max : n;
}

static inline float random_float(unsigned int *state) {
	return (float)rand_r(state) / ((float)RAND_MAX + 1.0f);
}

static inline float random_float_between(float min, float max, unsigned int *state) {
	float result = random_float(state)*(max - min) + min;
	return result;
}

static inline float degrees_to_radians(float degrees) {
	float rad = degrees * pi / 180.0f;
	return rad;
}

#include "vector3.h"

#endif
