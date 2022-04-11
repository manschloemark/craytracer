#ifndef COMMON_H
#define COMMON_H

#include <math.h>
#include <stdlib.h>

static inline float random_float() {
	return (float)rand() / (RAND_MAX + 1.0);
}

float random_float_between(float min, float max) {
	float result = random_float()*(max - min) + min;
	return result;
}

#include "vector3.h"

#endif
