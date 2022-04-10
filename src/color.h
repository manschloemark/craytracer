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

fcolor fcolor_new(float r, float g, float b) {
	fcolor c = {};
	c.r = r;
	c.g = g;
	c.b = b;
	return c;
}

// Returns a new color that is f% color b and the rest color a.

fcolor color_mul(fcolor a, float f) {
	fcolor c = {};
	c.r = a.r*f;
	c.g = a.g*f;
	c.b = a.b*f;
	return c;
}

fcolor color_mix(fcolor a, fcolor b, float f) {
	fcolor c = {};
	c.r = (1.0 - f) * a.r + f * b.r;
	c.g = (1.0 - f) * a.g + f * b.g;
	c.b = (1.0 - f) * a.b + f * b.b;
	return c;
}

#endif
