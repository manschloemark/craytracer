#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"
#include "texture.h"

// NOTE: Every "class" will have three functions:
// 1. <classname> <classname>_new(args) -- makes the struct with typedef <classname> and returns it
// texture make_<classname>(args) -- makes the "superclass" texture, sets the appropriate TextureID, and assigns union variable to result of <classname>_new()
// texture *add_<classname>(memory_region, args) -- calls make_<classname>() and then calls memory_region_add() to copy it into memory. Returns that pointer in the memory_region

// NOTE : classes with complex color calculations will have a function called <classname>TextureColor() which is called from TextureColor()
// some classes, like color_texture, are simple and don't need a function.

// Color Texture - a solid color
//
color_texture color_texture_new(fcolor color) {
	color_texture coltxt;
	coltxt.rgb = color;
	return coltxt;
}

texture make_color_texture(fcolor color) {
	texture coltxt = {};
	coltxt.type.color = color_texture_new(color);
	coltxt.id = Color;
	return coltxt;
}

texture *add_color_texture(memory_region *region, fcolor color) {
	texture coltxt = make_color_texture(color);
	texture *txtptr = (texture *)memory_region_add(region, &coltxt, sizeof(texture));
	return txtptr;
}

// Image Texture
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

texture make_image_texture(const char *filename, int bytes_per_pixel) {
	texture imgtxt = {};
	imgtxt.id = Image;
	imgtxt.type.image = image_texture_new(filename, bytes_per_pixel);
	return imgtxt;
}

texture *add_image_texture(memory_region *region, const char *filename, int bytes_per_pixel) {
	texture imgtxt = make_image_texture(filename, bytes_per_pixel);
	texture *txtptr = (texture *)memory_region_add(region, &imgtxt, sizeof(texture));
	return txtptr;
}

// UV-Coordinate Texture
texture make_uv_texture() {
	texture uvtxt = {};
	uvtxt.id = UV;
	uv_texture uv = {};
	uvtxt.type.uv = uv;
	return uvtxt;
}

texture *add_uv_texture(memory_region *region) {
	texture uvtxt = make_uv_texture();
	texture *txtptr = (texture *)memory_region_add(region, &uvtxt, sizeof(texture));
	return txtptr;
}

// Checker Texture -- takes two textures and returns them in a checker pattern 
checker_texture checker_texture_new(void *odd, void *even, float freq) {
	checker_texture txt = {};
	txt.odd = odd;
	txt.even = even;
	txt.freq = freq;
	return txt;
}

texture make_checker_texture(texture *odd, texture *even, float freq) {
	texture txt = {};
	checker_texture checkertxt = checker_texture_new(odd, even, freq);
	txt.id = Checker;
	txt.type.checker = checkertxt;
	return txt;
}

texture *add_checker_texture(memory_region *region, texture *odd, texture *even, float freq) {
	texture checkertxt = make_checker_texture(odd, even, freq);
	texture *checkerptr = (texture *)memory_region_add(region, &checkertxt, sizeof(texture));
	return checkerptr;
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

fcolor CheckerTextureColor(checker_texture *chtxt, float u, float v, vec3 pt) {
	float sines = sinf(chtxt->freq * pt.x) * sinf(chtxt->freq * pt.y) * sinf(chtxt->freq * pt.z);
	if (sines < 0)
		return TextureColor((texture *)chtxt->odd, u, v, pt);
	return TextureColor((texture *)chtxt->even, u, v, pt);
}

fcolor TextureColor(texture *text, float u, float v, point3 pt) {
	switch(text->id) {
		case Image:
			return ImageTextureColor(&text->type.image, u, v);
		case Color:
			return text->type.color.rgb;
		case UV:
			return fcolor_new(u, (u + v) / 2.0, v); // NOTE : This is not very useful tbh
		case Checker:
			return CheckerTextureColor(&text->type.checker, u, v, pt);
		default:
			return COLOR_UNDEFPURP;
	}
}
