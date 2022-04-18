#ifndef COMMON_H
#define COMMON_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

const float pi = 3.141592;

static inline float clamp(float n, float min, float max) {
	return (n < min) ? min : (n > max) ? max : n;
}

static inline float random_float() {
	return (float)rand() / (RAND_MAX + 1.0);
}

float random_float_between(float min, float max) {
	float result = random_float()*(max - min) + min;
	return result;
}

static inline float degrees_to_radians(float degrees) {
	float rad = degrees * pi / 180.0;
	return rad;
}

#include "vector3.h"

#endif
