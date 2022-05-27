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
	double freq;
} checker_texture;

checker_texture checker_texture_new(void *odd, void *even, double freq);

typedef struct {
	noise *noise;
	void *texture;
	double scale;
} noise_texture;

typedef struct {
	noise *noise;
	void *colA, *colB;
	double scale;
} gradient_noise_texture;

noise_texture noise_texture_new(memory_region *region, noise *noise, double scale, int pointcount, void *texture);

gradient_noise_texture gradient_noise_texture_new(memory_region *region, noise *perl, double scale, int pointcount, void *colA, void *colB);

typedef struct {
	noise *noise;
	void *text;
	double hurst;
	int octaves;
} fbm_texture;

fbm_texture fbm_modifier_new(memory_region *region, noise *noise, double hurst, int octaves, void *text);

typedef struct {
	vec3 intervals, widths;
	void *targetColor, *defaultColor;
} level_curve_texture;

static inline level_curve_texture level_curve_texture_new(vec3 intervals, vec3 widths, void *targetColor, void *defaultColor) {
	level_curve_texture lc = {intervals, widths, targetColor, defaultColor};
	return lc;
}

// Making this texture to help debug fbm offset shapes
// Specify a point and a cutoff distance. The color will be a gradient scaled by the distance to the point.
// Distance greater than the cutoff is white
typedef struct {
	point3 pt;
	double cutoff;
	void *textA, *textB;
} distance_texture;

static inline distance_texture distance_texture_new(vec3 point, double cutoff, void *textA, void *textB) {
	distance_texture dt = {point, cutoff, textA, textB};
	return dt;
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
	Distance,
};

union texture_type {
	color_texture color;
	image_texture image;
	uv_texture uv;
	checker_texture checker;
	noise_texture noise;
	gradient_noise_texture gradient_noise;
	fbm_texture fbm;
	level_curve_texture level_curve;
	distance_texture distance;
};

typedef struct {
	fcolor (*TextureColor) (void *self, double u, double v, point3 pt, vec3 *normal);
	union texture_type type;
	enum TextureID id;
} texture;

texture make_normal_texture();
texture *add_normal_texture(memory_region *region);
texture *add_signed_normal_texture(memory_region *region);

texture make_color_texture(fcolor color);
texture *add_color_texture(memory_region *region, fcolor color);

texture make_image_texture(const char *filename, int bytes_per_pixel);
texture *add_image_texture(memory_region *region, const char *filename, int bytes_per_pixel);

texture make_uv_texture();
texture *add_uv_texture(memory_region *region);

texture make_checker_texture(texture *odd, texture *even, double freq);
texture *add_checker_texture(memory_region *region, texture *odd, texture *even, double freq);

texture make_uv_checker_texture(texture *odd, texture *even, double freq);
texture *add_uv_checker_texture(memory_region *region, texture *odd, texture *even, double freq);

// This type of texture is unique in that make_noisee_noise_texture and noise_texture_new both require the memory region passed as well.
// I'm not happy with this because it makes everything else inconsistent.
// But I need to somehow get the actual noise struct into memory safely.
texture make_noise_texture(memory_region *region, noise *noise, double scale, int pointcount, texture *text);
texture *add_noise_texture(memory_region *region, noise *noise, double scale);
texture *add_noise_texture_sized(memory_region *region, double scale, int bits);

texture make_noise_turbulence_texture(memory_region *region, noise *noise, double scale, int pointcount, texture *text);
texture *add_noise_turbulence_texture(memory_region *region, noise *noise, double scale);
texture *add_noise_turbulence_texture_sized(memory_region *region, noise *noise, double scale, int bits);

texture *add_colored_noise_texture(memory_region *region, noise *noise, double scale, texture *text);
texture *add_colored_noise_texture_sized(memory_region *region, double scale, int bits, texture *text);

texture *add_colored_noise_turbulence_texture(memory_region *region, noise *noise, double scale, texture *text);
texture *add_colored_noise_turbulence_texture_sized(memory_region *region, double scale, int bits, texture *text);

texture *add_marbled_noise_texture(memory_region *region, noise *noise, double scale, texture *text);

texture make_gradient_noise_texture(memory_region *region, noise *noise, double scale, int pointcount, texture *colA, texture *colB);

texture *add_noise_sincos_texture(memory_region *region, noise *noise, double scale, texture *colA, texture *colB);

texture make_fbm_modifier(memory_region *region, noise *noise, double hurst, int octaves, texture *text);
texture *add_fbm_modifier(memory_region *region, noise *noise, double hurst, int octaves, texture *text);

texture make_level_curve_texture(vec3 intervals, vec3 widths, texture *targetColor, texture *defaltColor);
texture *add_level_curve_texture(memory_region *region, vec3 intervals, vec3 widths, texture *targetColor, texture *defaltColor);

texture make_distance_texture(point3 point, double cutoff, texture *textA, texture *textB);
texture *add_distance_texture(memory_region *region, point3 point, double cutoff, texture *textA, texture *textB);

fcolor UNDEFINED_TextureColor(void *self, double u, double v, point3 pt, vec3 *normal);
fcolor NormalTextureColor(void *self, double u, double v, point3 pt, vec3 *normal);
fcolor SignedNormalTextureColor(void *self, double u, double v, point3 pt, vec3 *normal);
fcolor ColorTextureColor(void *self, double u, double v, point3 pt, vec3 *normal);
fcolor ImageTextureColor(void *self, double u, double v, point3 pt, vec3 *normal);
fcolor CheckerTextureColor(void *self, double u, double v, point3 pt, vec3 *normal);
fcolor UVCheckerTextureColor(void *self, double u, double v, point3 pt, vec3 *normal);
fcolor NoiseTextureColor(void *self, double u, double v, point3 pt, vec3 *normal);
fcolor FBMTextureColor(void *self, double u, double v, point3 pt, vec3 *normal);
fcolor MarbledNoiseTextureColor(void *self, double u, double v, point3 pt, vec3 *normal);
fcolor NoiseSinCosTextureColor(void *self, double u, double v, point3 pt, vec3 *normal);
fcolor FBMModifierTextureColor(void *self, double u, double v, point3 pt, vec3 *normal);
fcolor LevelCurveTextureColor(void *self, double u, double v, vec3 pt, vec3 *normal);
fcolor DistanceTextureColor(void *self, double u, double v, vec3 pt, vec3 *normal);

#endif
