#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "color.h"
#include "memory.h"

typedef struct {
	fcolor rgb;
} color_texture;

color_texture color_texture_new(fcolor color);

typedef struct {
	unsigned char *pixels; // TODO : determine how I'll access the image
	int width, height;
	int bytes_per_pixel;
	int pitch;
} image_texture;

image_texture image_texture_new(const char *filename, int bytes_per_pixel);

// This does not need a constructor (atm)
typedef struct {
} uv_texture;

typedef struct {
	void *odd, *even;
	float freq;
} checker_texture;

checker_texture checker_texture_new(void *odd, void *even, float freq);

enum TextureID {
	Color,
	Image,
	UV,
	Checker,
};

union texture_type {
	color_texture color;
	image_texture image;
	uv_texture uv;
	checker_texture checker;
};

typedef struct {
	union texture_type type;
	enum TextureID id;
} texture;

texture make_color_texture(fcolor color);
texture *add_color_texture(memory_region *region, fcolor color);

texture make_image_texture(const char *filename, int bytes_per_pixel);
texture *add_image_texture(memory_region *region, const char *filename, int bytes_per_pixel);

texture make_uv_texture();
texture *add_uv_texture(memory_region *region);

texture make_checker_texture(texture *odd, texture *even, float freq);
texture *add_checker_texture(memory_region *region, texture *odd, texture *even, float freq);

fcolor TextureColor(texture *text, float u, float v, point3 pt);

#endif
