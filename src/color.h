#ifndef COLOR_H
#define COLOR_H

#include "common.h"

struct Color {
	float r, g, b;
};
typedef struct Color fcolor;

#define COLOR_ADD(macro_dest, macro_color) \
	(macro_dest).r += (macro_color).r; \
  (macro_dest).g += (macro_color).g; \
  (macro_dest).b += (macro_color).b;

#define COLOR_MUL(macro_dest, macro_color) \
	(macro_dest).r *= (macro_color).r; \
  (macro_dest).g *= (macro_color).g; \
  (macro_dest).b *= (macro_color).b;

static inline fcolor fcolor_new(float r, float g, float b) {
	fcolor c = {r, g, b};
	return c;
}

static inline fcolor color_mul(fcolor a, float f) {
	fcolor c = {};
	c.r = a.r*f;
	c.g = a.g*f;
	c.b = a.b*f;
	return c;
}

static inline fcolor color_normalize(fcolor a) {
	// This is kind of silly but it's just easy for now
	float len = vec3_len(vec3_new(a.r, a.g, a.b));
	return fcolor_new(a.r / len, a.g / len, a.b / len);
}

// Returns a new color that is f% color b and the rest color a.
static inline fcolor color_mix(fcolor a, fcolor b, float f) {
	fcolor c = {};
	c.r = (1.0 - f) * a.r + f * b.r;
	c.g = (1.0 - f) * a.g + f * b.g;
	c.b = (1.0 - f) * a.b + f * b.b;
	return c;
}

// Some macros for common colors
#define COLOR_RED fcolor_new(1.0, 0.0, 0.0)
#define COLOR_GREEN fcolor_new(0.0, 1.0, 0.0)
#define COLOR_BLUE fcolor_new(0.0, 0.0, 1.0)
#define COLOR_BLACK fcolor_new(0.0, 0.0, 0.0)
#define COLOR_WHITE fcolor_new(1.0, 1.0, 1.0)
#define COLOR_VALUE(f) fcolor_new(f, f, f)
// Color to be used when textures go bad or something
#define COLOR_UNDEFPURP fcolor_new(1.0, 0.65, 1.0)


#endif
