#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "color.h"
#include "stb_image_include.h"
#include "memory.h"

typedef struct {
	fcolor rgb;
} color_texture;

color_texture color_texture_new(fcolor color) {
	color_texture coltxt;
	coltxt.rgb = color;
	return coltxt;
}

typedef struct {
	unsigned char *pixels; // TODO : determine how I'll access the image
	int width, height;
	int bytes_per_pixel;
	int pitch;
} image_texture;

image_texture image_texture_new(const char *filename, int bytes_per_pixel) {
	image_texture imgtxt = {};
	int width = 0;
	int height = 0;
	imgtxt.pixels = stbi_load(filename, &width, &height, &bytes_per_pixel, bytes_per_pixel);
	imgtxt.width = width;
	imgtxt.height = height;
	imgtxt.bytes_per_pixel = bytes_per_pixel;
	imgtxt.pitch = bytes_per_pixel * width;
	if (!imgtxt.pixels) {
		printf("Error loading file %s for image_texture.\n", filename);
		imgtxt.width = 0;
		imgtxt.height = 0;
	}
	return imgtxt;
}

fcolor ImageTextureColor(image_texture *imgtxt, float u, float v) {
	if (!imgtxt->pixels) return COLOR_UNDEFPURP;
	int x = clamp(u, 0.0, 1.0) * (float)imgtxt->width;
	int y = imgtxt->height - clamp(v, 0.0, 1.0) * (float)imgtxt->height;
	if(x >= imgtxt->width) x = imgtxt->width - 1;
	if(y >= imgtxt->height) y = imgtxt->height - 1;

	unsigned char *pixel = imgtxt->pixels + (y * imgtxt->pitch) + (x * imgtxt->bytes_per_pixel);
	fcolor result = fcolor_new((float)*pixel / 255.0, (float)*(pixel + 1) / 255.0, (float)*(pixel + 2) / 255.0);
	return result;
}

typedef struct {
} uv_texture;

enum TextureID {
	Color,
	Image,
	UV,
};

union texture_type {
	color_texture color;
	image_texture image;
	uv_texture uv;
};

typedef struct {
	union texture_type type;
	enum TextureID id;
} texture;

texture make_color_texture(fcolor color) {
	texture coltxt = {};
	coltxt.type.color = color_texture_new(color);
	coltxt.id = Color;
	return coltxt;
}

texture make_image_texture(const char *filename, int bytes_per_pixel) {
	texture imgtxt = {};
	imgtxt.id = Image;
	imgtxt.type.image = image_texture_new(filename, bytes_per_pixel);
	return imgtxt;
}

texture make_uv_texture() {
	texture uvtxt = {};
	uvtxt.id = UV;
	uv_texture uv = {};
	uvtxt.type.uv = uv;
	return uvtxt;
}

texture *add_color_texture(memory_region *region, fcolor color) {
	texture coltxt = make_color_texture(color);
	texture *txtptr = (texture *)memory_region_add(region, &coltxt, sizeof(texture));
	return txtptr;
}

texture *add_image_texture(memory_region *region, const char *filename, int bytes_per_pixel) {
	texture imgtxt = make_image_texture(filename, bytes_per_pixel);
	texture *txtptr = (texture *)memory_region_add(region, &imgtxt, sizeof(texture));
	return txtptr;
}

texture *add_uv_texture(memory_region *region) {
	texture uvtxt = make_uv_texture();
	texture *txtptr = (texture *)memory_region_add(region, &uvtxt, sizeof(texture));
	return txtptr;
}

fcolor TextureColor(texture *text, float u, float v, point3 pt) {
	switch(text->id) {
		case Image:
			return ImageTextureColor(&text->type.image, u, v);
		case Color:
			return text->type.color.rgb;
		case UV:
			return fcolor_new(u, (u + v) / 2.0, v); // NOTE : This is not very useful tbh
		default:
			return COLOR_UNDEFPURP;
	}
}
#endif
