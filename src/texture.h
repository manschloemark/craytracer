#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "color.h"
#include "memory.h"
#include "perlin_noise.h"

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

typedef struct {
	perlin *perlin;
	void *texture;
	float scale;
} perlin_texture;

typedef struct {
	perlin *perlin;
	void *colA, *colB;
	float scale;
} multicolor_perlin_texture;

perlin_texture perlin_texture_new(memory_region *region, float scale, int pointcount, void *texture);

multicolor_perlin_texture multicolor_perlin_texture_new(memory_region *region, float scale, int pointcount, void *colA, void *colB);

enum TextureID {
	Undefined,
	Color,
	Image,
	UV,
	Checker,
	UVChecker,
	PerlinNoise,
	PerlinTurbulence,
	PerlinMarbled,
	PerlinSinCos,
};

union texture_type {
	color_texture color;
	image_texture image;
	uv_texture uv;
	checker_texture checker;
	perlin_texture perlin;
	multicolor_perlin_texture multicolor_perlin;
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

texture make_uv_checker_texture(texture *odd, texture *even, float freq);
texture *add_uv_checker_texture(memory_region *region, texture *odd, texture *even, float freq);

// This type of texture is unique in that make_perline_noise_texture and perlin_noise_texture_new both require the memory region passed as well.
// I'm not happy with this because it makes everything else inconsistent.
// But I need to somehow get the actual perlin struct into memory safely.
texture make_perlin_noise_texture(memory_region *region, float scale, int pointcount, texture *text);
texture *add_perlin_noise_texture(memory_region *region, float scale);
texture *add_perlin_noise_texture_sized(memory_region *region, float scale, int bits);

texture make_perlin_turbulence_texture(memory_region *region, float scale, int pointcount, texture *text);
texture *add_perlin_turbulence_texture(memory_region *region, float scale);
texture *add_perlin_turbulence_texture_sized(memory_region *region, float scale, int bits);

texture *add_colored_perlin_noise_texture(memory_region *region, float scale, texture *text);
texture *add_colored_perlin_noise_texture_sized(memory_region *region, float scale, int bits, texture *text);

texture *add_colored_perlin_turbulence_texture(memory_region *region, float scale, texture *text);
texture *add_colored_perlin_turbulence_texture_sized(memory_region *region, float scale, int bits, texture *text);

texture *add_marbled_noise_texture(memory_region *region, float scale, texture *text);

texture make_multicolor_perlin_texture(memory_region *region, float scale, int pointcount, texture *colA, texture *colB);

texture *add_perlin_sincos_texture(memory_region *regoin, float scale, texture *colA, texture *colB);

fcolor TextureColor(texture *text, float u, float v, point3 pt);

#endif
