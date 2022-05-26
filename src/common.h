#ifndef COMMON_H
#define COMMON_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
	int id;
	struct drand48_data rand_state;
} thread_context;

static const double pi   = 3.141592;
static const double pi_2 = 1.570796;

static inline double clamp(double n, double min, double max) {
	return (n < min) ? min : (n > max) ? max : n;
}

static inline double random_double(struct drand48_data *state) {
	double result;
	drand48_r(state, &result);
	return result;
}

static inline double random_double_between(double min, double max, struct drand48_data *state) {
	double result = random_double(state)*(max - min) + min;
	return result;
}

static inline double degrees_to_radians(double degrees) {
	double rad = degrees * pi / 180.0;
	return rad;
}

#include "vector3.h"

#endif
