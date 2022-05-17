#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "color.h"
#include "memory.h"
#include "noise.h"

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
	noise *noise;
	void *texture;
	float scale;
} noise_texture;

typedef struct {
	noise *noise;
	void *colA, *colB;
	float scale;
} gradient_noise_texture;

noise_texture noise_texture_new(memory_region *region, noise *noise, float scale, int pointcount, void *texture);

gradient_noise_texture gradient_noise_texture_new(memory_region *region, noise *perl, float scale, int pointcount, void *colA, void *colB);

typedef struct {
	noise *noise;
	void *text;
	float hurst;
	int octaves;
} fbm_modifier;

fbm_modifier fbm_modifier_new(memory_region *region, noise *noise, float hurst, int octaves, void *text);

typedef struct {
	vec3 intervals, widths;
	void *targetColor, *defaultColor;
} level_curve_texture;

static inline level_curve_texture level_curve_texture_new(vec3 intervals, vec3 widths, void *targetColor, void *defaultColor) {
	level_curve_texture lc = {intervals, widths, targetColor, defaultColor};
	return lc;
}

enum TextureID {
	Undefined,
	Normal,
	Color,
	Image,
	UV,
	Checker,
	UVChecker,
	Noise,
	FBM,
	MarbledNoise,
	NoiseSinCos,
	FBMMod,
	LevelCurves,
};

union texture_type {
	color_texture color;
	image_texture image;
	uv_texture uv;
	checker_texture checker;
	noise_texture noise;
	gradient_noise_texture gradient_noise;
	fbm_modifier fbm_mod;
	level_curve_texture level_curve;
};

typedef struct {
	fcolor (*TextureColor) (void *self, float u, float v, point3 pt, vec3 *normal);
	union texture_type type;
	enum TextureID id;
} texture;

texture make_normal_texture();
texture *add_normal_texture(memory_region *region);

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

// This type of texture is unique in that make_noisee_noise_texture and noise_texture_new both require the memory region passed as well.
// I'm not happy with this because it makes everything else inconsistent.
// But I need to somehow get the actual noise struct into memory safely.
texture make_noise_texture(memory_region *region, noise *noise, float scale, int pointcount, texture *text);
texture *add_noise_texture(memory_region *region, noise *noise, float scale);
texture *add_noise_texture_sized(memory_region *region, float scale, int bits);

texture make_noise_turbulence_texture(memory_region *region, noise *noise, float scale, int pointcount, texture *text);
texture *add_noise_turbulence_texture(memory_region *region, noise *noise, float scale);
texture *add_noise_turbulence_texture_sized(memory_region *region, noise *noise, float scale, int bits);

texture *add_colored_noise_texture(memory_region *region, noise *noise, float scale, texture *text);
texture *add_colored_noise_texture_sized(memory_region *region, float scale, int bits, texture *text);

texture *add_colored_noise_turbulence_texture(memory_region *region, noise *noise, float scale, texture *text);
texture *add_colored_noise_turbulence_texture_sized(memory_region *region, float scale, int bits, texture *text);

texture *add_marbled_noise_texture(memory_region *region, noise *noise, float scale, texture *text);

texture make_gradient_noise_texture(memory_region *region, noise *noise, float scale, int pointcount, texture *colA, texture *colB);

texture *add_noise_sincos_texture(memory_region *region, noise *noise, float scale, texture *colA, texture *colB);

texture make_fbm_modifier(memory_region *region, noise *noise, float hurst, int octaves, texture *text);
texture *add_fbm_modifier(memory_region *region, noise *noise, float hurst, int octaves, texture *text);

texture make_level_curve_texture(vec3 intervals, vec3 widths, texture *targetColor, texture *defaltColor);
texture *add_level_curve_texture(memory_region *region, vec3 intervals, vec3 widths, texture *targetColor, texture *defaltColor);

fcolor UNDEFINED_TextureColor(void *self, float u, float v, point3 pt, vec3 *normal);
fcolor NormalTextureColor(void *self, float u, float v, point3 pt, vec3 *normal);
fcolor ColorTextureColor(void *self, float u, float v, point3 pt, vec3 *normal);
fcolor ImageTextureColor(void *self, float u, float v, point3 pt, vec3 *normal);
fcolor CheckerTextureColor(void *self, float u, float v, point3 pt, vec3 *normal);
fcolor UVCheckerTextureColor(void *self, float u, float v, point3 pt, vec3 *normal);
fcolor NoiseTextureColor(void *self, float u, float v, point3 pt, vec3 *normal);
fcolor FBMTextureColor(void *self, float u, float v, point3 pt, vec3 *normal);
fcolor MarbledNoiseTextureColor(void *self, float u, float v, point3 pt, vec3 *normal);
fcolor NoiseSinCosTextureColor(void *self, float u, float v, point3 pt, vec3 *normal);
fcolor FBMModifierTextureColor(void *self, float u, float v, point3 pt, vec3 *normal);
fcolor LevelCurveTextureColor(void *self, float u, float v, vec3 pt, vec3 *normal);

#endif
