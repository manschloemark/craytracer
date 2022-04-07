#ifndef COLOR_H
#define COLOR_H

struct Color {
	float r, g, b;
};
typedef struct Color color;

#define COLOR_ADD(macro_dest, macro_color) \
	(macro_dest).r += (macro_color).r; \
  (macro_dest).g += (macro_color).g; \
  (macro_dest).b += (macro_color).b;

color color_new(float r, float g, float b) {
	color c;
	c.r = r;
	c.g = g;
	c.b = b;
	return c;
}

// Returns a new color that is f% color b and the rest color a.
color color_mix(color a, color b, float f) {
	color c = {};
	c.r = (1.0 - f) * a.r + f * b.r;
	c.g = (1.0 - f) * a.g + f * b.g;
	c.b = (1.0 - f) * a.b + f * b.b;
	return c;
}

#endif
