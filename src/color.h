#ifndef COLOR_H
#define COLOR_H

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

fcolor fcolor_new(float r, float g, float b) {
	fcolor c = {};
	c.r = r;
	c.g = g;
	c.b = b;
	return c;
}

static inline fcolor color_mul(fcolor a, float f) {
	fcolor c = {};
	c.r = a.r*f;
	c.g = a.g*f;
	c.b = a.b*f;
	return c;
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
// Color to be used when textures go bad or something
#define COLOR_UNDEFPURP fcolor_new(1.0, 0.65, 1.0);


#endif
