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

// UV Checker Texture -- this is like the Checker Texture from Peter Shirley's book, but instead of using xyz coordinates it uses uv to make checkers
// this allows flat shapes to have tiles
// Since it's identical in every way, this will just use checker_texture_new

texture make_uv_checker_texture(texture *odd, texture *even, float freq) {
	texture txt = {};
	checker_texture checkertxt = checker_texture_new(odd, even, freq);
	txt.id = UVChecker;
	txt.type.checker = checkertxt;
	return txt;
}

texture *add_uv_checker_texture(memory_region *region, texture *odd, texture *even, float freq) {
	texture checkertxt = make_uv_checker_texture(odd, even, freq);
	texture *checkerptr = (texture *)memory_region_add(region, &checkertxt, sizeof(texture));
	return checkerptr;
}

// Perlin Noise Texture
perlin_texture perlin_texture_new(memory_region *region, float scale, int pointcount, void *texture) {
	perlin_texture p = {};
	p.perlin = add_perlin(region, pointcount);
	p.texture = texture;
	p.scale = scale;
	return p;
}

texture make_perlin_noise_texture(memory_region *region, float scale, int pointcount, texture *text) {
	texture txt = {};
	perlin_texture perl = perlin_texture_new(region, scale, pointcount, text);
	txt.id = PerlinNoise;
	txt.type.perlin = perl;
	return txt;
}

texture *add_perlin_noise_texture(memory_region *region, float scale) {
	// NOTE : this is not good practice imo, but for convencience sake
	texture perltext = make_perlin_noise_texture(region, scale, 256, NULL);
	texture *perlptr = (texture *)memory_region_add(region, &perltext, sizeof(texture));
	return perlptr;
}

texture *add_colored_perlin_noise_texture(memory_region *region, float scale, texture *text) {
	// NOTE : this is not good practice imo, but for convencience sake
	texture perltext = make_perlin_noise_texture(region, scale, 256, text);
	texture *perlptr = (texture *)memory_region_add(region, &perltext, sizeof(texture));
	return perlptr;
}

// Since C does not have function overloading
// this is an alternive way to create a perlin_noise_texture.
// Pass bits (must be in [1 - 10]) and the internal perlin structure will 
// use that many points.
// It basically determines how often the pattern repeats.
texture *add_perlin_noise_texture_sized(memory_region *region, float scale, int bits) {
	// NOTE : this is not good practice imo, but for convencience sake
	int pointcount = (bits > 10 || bits <= 0) ? 256 : pow(2, bits);
	texture perltext = make_perlin_noise_texture(region, scale, pointcount, NULL);
	texture *perlptr = (texture *)memory_region_add(region, &perltext, sizeof(texture));
	return perlptr;
}

texture *add_colored_perlin_noise_texture_sized(memory_region *region, float scale, int bits, texture *text) {
	// NOTE : this is not good practice imo, but for convencience sake
	int pointcount = (bits > 10 || bits <= 0) ? 256 : pow(2, bits);
	texture perltext = make_perlin_noise_texture(region, scale, pointcount, text);
	texture *perlptr = (texture *)memory_region_add(region, &perltext, sizeof(texture));
	return perlptr;
}

// Perlin Turbulence -- same as perlin_noise_texture but calls perlin_turbulence instead of perlin_noise
texture *add_perlin_turbulence_texture(memory_region *region, float scale) {
	texture perltext = make_perlin_noise_texture(region, scale, 256, NULL);
	perltext.id = PerlinTurbulence;
	texture *perlptr = (texture *)memory_region_add(region, &perltext, sizeof(texture));
	return perlptr;
}

texture *add_colored_perlin_turbulence_texture(memory_region *region, float scale, texture *text) {
	texture perltext = make_perlin_noise_texture(region, scale, 256, text);
	perltext.id = PerlinTurbulence;
	texture *perlptr = (texture *)memory_region_add(region, &perltext, sizeof(texture));
	return perlptr;
}

texture *add_marbled_noise_texture(memory_region *region, float scale, texture *text) {
	texture perltext = make_perlin_noise_texture(region, scale, 256, text);
	perltext.id = PerlinMarbled;
	texture *perlptr = (texture *)memory_region_add(region, &perltext, sizeof(texture));
	return perlptr;
}

// Multicolor Perlin Textures -- perlin textures that use two colors.
multicolor_perlin_texture multicolor_perlin_texture_new(memory_region *region, float scale, int pointcount, void *colA, void *colB) {
	multicolor_perlin_texture p = {};
	p.perlin = add_perlin(region, pointcount);
	p.colA = colA;
	p.colB = colB;
	p.scale = scale;
	return p;
}

texture make_multicolor_perlin_texture(memory_region *region, float scale, int pointcount, texture *colA, texture *colB) {
	texture txt = {};
	multicolor_perlin_texture perl = multicolor_perlin_texture_new(region, scale, pointcount, colA, colB);
	txt.id = Undefined;
	txt.type.multicolor_perlin = perl;
	return txt;
}

texture *add_perlin_sincos_texture(memory_region *region, float scale, texture *colA, texture *colB) {
	int pointcount = 256;
	texture mp = make_multicolor_perlin_texture(region, scale, pointcount, colA, colB);
	mp.id = PerlinSinCos;
	texture *mptr = (texture *)memory_region_add(region, &mp, sizeof(texture));
	return mptr;
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
	if (sines < 0.0)
		return TextureColor((texture *)chtxt->odd, u, v, pt);
	return TextureColor((texture *)chtxt->even, u, v, pt);
}

fcolor UVCheckerTextureColor(checker_texture *chtxt, float u, float v, vec3 pt) {
	float sines = sinf(chtxt->freq * u) * sinf(chtxt->freq * v);
	if (sines < 0.0)
		return TextureColor((texture *)chtxt->odd, u, v, pt);
	return TextureColor((texture *)chtxt->even, u, v, pt);
}

fcolor PerlinNoiseTextureColor(perlin_texture *perl, float u, float v, vec3 pt) {
	fcolor col;
	if (perl->texture != NULL) {
		col = TextureColor(perl->texture, u, v, pt);
	} else {
		col = fcolor_new(1.0, 1.0, 1.0);
	}
	point3 scaled_pt = vec3_mul(pt, perl->scale);
	float noise = (1.0 + perlin_noise(perl->perlin, &scaled_pt)) * 0.5;
	return color_mul(col, noise);
}

fcolor PerlinTurbulenceTextureColor(perlin_texture *perl, float u, float v, vec3 pt) {
	int depth = 7;
	fcolor col;
	if (perl->texture != NULL) {
		col = TextureColor(perl->texture, u, v, pt);
	} else {
		col = fcolor_new(1.0, 1.0, 1.0);
	}

	point3 scaled_pt = vec3_mul(pt, perl->scale);
	return color_mul(col, perlin_turbulence(perl->perlin, &pt, depth));
}

fcolor PerlinMarbledTextureColor(perlin_texture *perl, float u, float v, vec3 pt) {
	int depth = 7;
	fcolor col;
	if (perl->texture != NULL) {
		col = TextureColor(perl->texture, u, v, pt);
	} else {
		col = fcolor_new(1.0, 1.0, 1.0);
	}
	point3 scaled_pt = vec3_mul(pt, perl->scale);
	float noise = 0.5 * (1.0 + sinf(scaled_pt.y + 10.0 * perlin_turbulence(perl->perlin, &pt, depth)));
	return color_mul(col, noise);
}

fcolor PerlinSinCosTextureColor(multicolor_perlin_texture *perl, float u, float v, vec3 pt) {
	int depth = 7;
	vec3 swizzled = vec3_new(pt.z, pt.x, pt.y);
	float turb = sinf(perl->scale * 10.0 * perlin_turbulence(perl->perlin, &pt, depth));
	turb *= cosf(perl->scale * perlin_turbulence(perl->perlin, &swizzled, depth));
	if (turb > 0.0) {
		return color_mul(TextureColor(perl->colA, u, v, pt), turb);
	} else {
		return color_mul(TextureColor(perl->colB, u, v, pt), fabs(turb));
	}
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
		case UVChecker:
			return UVCheckerTextureColor(&text->type.checker, u, v, pt);
		case PerlinNoise:
			return PerlinNoiseTextureColor(&text->type.perlin, u, v, pt);
		case PerlinTurbulence:
			return PerlinTurbulenceTextureColor(&text->type.perlin, u, v, pt);
		case PerlinMarbled:
			return PerlinMarbledTextureColor(&text->type.perlin, u, v, pt);
		case PerlinSinCos:
			return PerlinSinCosTextureColor(&text->type.multicolor_perlin, u, v, pt);
		default:
			return COLOR_UNDEFPURP;
	}
}
