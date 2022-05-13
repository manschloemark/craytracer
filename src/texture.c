#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"
#include "texture.h"

// NOTE: Every "class" will have three functions:
// 1. <classname> <classname>_new(args) -- makes the struct with typedef <classname> and returns it
// texture make_<classname>(args) -- makes the "superclass" texture, sets the appropriate TextureID, and assigns union variable to result of <classname>_new()
// texture *add_<classname>(memory_region, args) -- calls make_<classname>() and then calls memory_region_add() to copy it into memory. Returns that pointer in the memory_region

// NOTE : classes with complex color calculations will have a function called <classname>TextureColor() which is called from TextureColor()
// some classes, like color_texture, are simple and don't need a function.

// NOTE : there is really no need to have an underlying normal_texture struct
texture make_normal_texture() {
	texture txt = {};
	color_texture empty_texture = {};
	txt.TextureColor = (*NormalTextureColor);
	txt.id = Normal;
	txt.type.color = empty_texture; 
	return txt;
}

texture *add_normal_texture(memory_region *region) {
	texture normtxt = make_normal_texture();
	return (texture *)memory_region_add(region, &normtxt, sizeof(texture));
}

// Color Texture - a solid color
//
color_texture color_texture_new(fcolor color) {
	color_texture coltxt;
	coltxt.rgb = color;
	return coltxt;
}

texture make_color_texture(fcolor color) {
	texture coltxt = {};
	coltxt.TextureColor = (*ColorTextureColor);
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
	imgtxt.TextureColor = (*ImageTextureColor);
	imgtxt.id = Image;
	imgtxt.type.image = image_texture_new(filename, bytes_per_pixel);
	return imgtxt;
}

texture *add_image_texture(memory_region *region, const char *filename, int bytes_per_pixel) {
	texture imgtxt = make_image_texture(filename, bytes_per_pixel);
	texture *txtptr = (texture *)memory_region_add(region, &imgtxt, sizeof(texture));
	return txtptr;
}

// TODO : Implement these
// UV-Coordinate Texture
texture make_uv_texture() {
	texture uvtxt = {};
	uvtxt.TextureColor = (*UNDEFINED_TextureColor);
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
	txt.TextureColor = (*CheckerTextureColor);
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
	txt.TextureColor = (*UVCheckerTextureColor);
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
perlin_texture perlin_texture_new(memory_region *region, perlin *perl, float scale, int pointcount, void *texture) {
	perlin_texture p = {};
	p.perlin = (perl == NULL) ? add_perlin(region, pointcount) : perl;
	p.texture = texture;
	p.scale = scale;
	return p;
}

texture make_perlin_noise_texture(memory_region *region, perlin *perl, float scale, int pointcount, texture *text) {
	texture txt = {};
	txt.TextureColor = (*PerlinNoiseTextureColor);
	perlin_texture perltxt = perlin_texture_new(region, perl, scale, pointcount, text);
	txt.id = PerlinNoise;
	txt.type.perlin = perltxt;
	return txt;
}

texture *add_perlin_noise_texture(memory_region *region, perlin *perl, float scale) {
	// NOTE : this is not good practice imo, but for convencience sake
	texture perltext = make_perlin_noise_texture(region, perl, scale, 256, NULL);
	texture *perlptr = (texture *)memory_region_add(region, &perltext, sizeof(texture));
	return perlptr;
}

texture *add_colored_perlin_noise_texture(memory_region *region, perlin *perl, float scale, texture *text) {
	// NOTE : this is not good practice imo, but for convencience sake
	texture perltext = make_perlin_noise_texture(region, perl, scale, 256, text);
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
	texture perltext = make_perlin_noise_texture(region, NULL, scale, pointcount, NULL);
	texture *perlptr = (texture *)memory_region_add(region, &perltext, sizeof(texture));
	return perlptr;
}

texture *add_colored_perlin_noise_texture_sized(memory_region *region, float scale, int bits, texture *text) {
	// NOTE : this is not good practice imo, but for convencience sake
	int pointcount = (bits > 10 || bits <= 0) ? 256 : pow(2, bits);
	texture perltext = make_perlin_noise_texture(region, NULL, scale, pointcount, text);
	texture *perlptr = (texture *)memory_region_add(region, &perltext, sizeof(texture));
	return perlptr;
}

// Perlin Turbulence -- same as perlin_noise_texture but calls perlin_turbulence instead of perlin_noise
texture *add_perlin_turbulence_texture(memory_region *region, perlin *perl, float scale) {
	texture perltext = make_perlin_noise_texture(region, perl, scale, 256, NULL);
	perltext.TextureColor = (*PerlinTurbulenceTextureColor);
	perltext.id = PerlinTurbulence;
	texture *perlptr = (texture *)memory_region_add(region, &perltext, sizeof(texture));
	return perlptr;
}

texture *add_colored_perlin_turbulence_texture(memory_region *region, perlin *perl, float scale, texture *text) {
	texture perltext = make_perlin_noise_texture(region, perl, scale, 256, text);
	perltext.TextureColor = (*PerlinTurbulenceTextureColor);
	perltext.id = PerlinTurbulence;
	texture *perlptr = (texture *)memory_region_add(region, &perltext, sizeof(texture));
	return perlptr;
}

texture *add_marbled_noise_texture(memory_region *region, perlin *perl, float scale, texture *text) {
	texture perltext = make_perlin_noise_texture(region, perl, scale, 256, text);
	perltext.TextureColor = (*PerlinMarbledTextureColor);
	perltext.id = PerlinMarbled;
	texture *perlptr = (texture *)memory_region_add(region, &perltext, sizeof(texture));
	return perlptr;
}

// Multicolor Perlin Textures -- perlin textures that use two colors.
multicolor_perlin_texture multicolor_perlin_texture_new(memory_region *region, perlin *perl, float scale, int pointcount, void *colA, void *colB) {
	multicolor_perlin_texture p = {};
	p.perlin = (perl == NULL) ? add_perlin(region, pointcount) : perl;
	p.colA = colA;
	p.colB = colB;
	p.scale = scale;
	return p;
}

texture make_multicolor_perlin_texture(memory_region *region, perlin *perl, float scale, int pointcount, texture *colA, texture *colB) {
	texture txt = {};
	txt.TextureColor = (*UNDEFINED_TextureColor);
	multicolor_perlin_texture perltxt = multicolor_perlin_texture_new(region, perl, scale, pointcount, colA, colB);
	txt.id = Undefined;
	txt.type.multicolor_perlin = perltxt;
	return txt;
}

texture *add_perlin_sincos_texture(memory_region *region, perlin *perl, float scale, texture *colA, texture *colB) {
	int pointcount = 256;
	texture mp = make_multicolor_perlin_texture(region, perl, scale, pointcount, colA, colB);
	mp.TextureColor = (*PerlinSinCosTextureColor);
	mp.id = PerlinSinCos;
	texture *mptr = (texture *)memory_region_add(region, &mp, sizeof(texture));
	return mptr;
}


// FBM Modifier -- take some other texture and return the value when you mess with the point
fbm_modifier fbm_modifier_new(memory_region *region, perlin *perl, float hurst, int octaves, void *text) {
	fbm_modifier fbm_mod = {};
	fbm_mod.hurst = hurst;
	fbm_mod.octaves = octaves;
	fbm_mod.text = text;
	if (perl) {
		fbm_mod.perlin = perl;
	} else {
		fbm_mod.perlin = add_perlin(region, 256);
	}
	return fbm_mod;
}

texture make_fbm_modifier(memory_region *region, perlin *perl, float hurst, int octaves, texture *text) {
	texture txt = {};
	txt.TextureColor = (*FBMModifierTextureColor);
	fbm_modifier fbm_mod = fbm_modifier_new(region, perl, hurst, octaves, text);
	txt.type.fbm_mod = fbm_mod;
	txt.id = FBM;
	return txt;
}
texture *add_fbm_modifier(memory_region *region, perlin *perl, float hurst, int octaves, texture *text) {
	texture fbmtxt = make_fbm_modifier(region, perl, hurst, octaves, text);
	texture *fbmptr = (texture *)memory_region_add(region, &fbmtxt, sizeof(texture));
	return fbmptr;
}

// Level Curves
texture make_level_curve_texture(vec3 intervals, vec3 widths, texture *targetColor, texture *defaultColor) {
	texture lctxt = {};
	lctxt.TextureColor = (*LevelCurveTextureColor);
	lctxt.type.level_curve = level_curve_texture_new(intervals, widths, targetColor, defaultColor);
	lctxt.id = LevelCurves;
	return lctxt;
}

texture *add_level_curve_texture(memory_region *region, vec3 intervals, vec3 widths, texture *targetColor, texture *defaultColor) {
	texture lct = make_level_curve_texture(intervals, widths, targetColor, defaultColor);
	return (texture *)memory_region_add(region, &lct, sizeof(texture));
}

// Assume all normals are unit vectors already. Pretty sure they are.
fcolor NormalTextureColor(void *self, float u, float v, vec3 pt, vec3 *normal) {
	fcolor norm_color = fcolor_new((1.0 + normal->x) / 2.0, (1.0 + normal->y) / 2.0, (1.0 + normal->z));
	return norm_color;
}

fcolor ColorTextureColor(void *self, float u, float v, vec3 pt, vec3 *normal) {
	color_texture *txt = &((texture *)self)->type.color; 
	return txt->rgb;
}

fcolor ImageTextureColor(void *self, float u, float v, point3 pt, vec3 *normal) {
	image_texture *imgtxt = &((texture *)self)->type.image;
	if (!imgtxt->pixels) return COLOR_UNDEFPURP;
	int x = clamp(u, 0.0, 1.0) * (float)imgtxt->width;
	int y = imgtxt->height - clamp(v, 0.0, 1.0) * (float)imgtxt->height;
	if(x >= imgtxt->width) x = imgtxt->width - 1;
	if(y >= imgtxt->height) y = imgtxt->height - 1;

	unsigned char *pixel = imgtxt->pixels + (y * imgtxt->pitch) + (x * imgtxt->bytes_per_pixel);
	fcolor result = fcolor_new((float)*pixel / 255.0, (float)*(pixel + 1) / 255.0, (float)*(pixel + 2) / 255.0);
	return result;
}

fcolor CheckerTextureColor(void *self, float u, float v, vec3 pt, vec3 *normal) {
	checker_texture *chtxt = &((texture *)self)->type.checker;
	float sines = sinf(chtxt->freq * pt.x) * sinf(chtxt->freq * pt.y) * sinf(chtxt->freq * pt.z);
	if (sines < 0.0)
		return ((texture *)chtxt->odd)->TextureColor(chtxt->odd, u, v, pt, normal);
	return ((texture *)chtxt->even)->TextureColor(chtxt->even, u, v, pt, normal);
}

fcolor UVCheckerTextureColor(void *self, float u, float v, vec3 pt, vec3 *normal) {
	checker_texture *chtxt = &((texture *)self)->type.checker;
	float sines = sinf(chtxt->freq * u) * sinf(chtxt->freq * v);
	if (sines < 0.0)
		return ((texture *)chtxt->odd)->TextureColor(chtxt->odd, u, v, pt, normal);
	return ((texture *)chtxt->even)->TextureColor(chtxt->even, u, v, pt, normal);
}

fcolor PerlinNoiseTextureColor(void *self, float u, float v, vec3 pt, vec3 *normal) {
	perlin_texture *perl = &((texture *)self)->type.perlin;
	fcolor col;
	if (perl->texture != NULL) {
		col = ((texture *)perl->texture)->TextureColor(perl->texture, u, v, pt, normal);
	} else {
		col = fcolor_new(1.0, 1.0, 1.0);
	}
	point3 scaled_pt = vec3_mul(pt, perl->scale);
	float noise = (1.0 + perlin_noise(perl->perlin, &scaled_pt)) * 0.5;
	return color_mul(col, noise);
}

fcolor PerlinTurbulenceTextureColor(void *self, float u, float v, vec3 pt, vec3 *normal) {
	perlin_texture *perl = &((texture *)self)->type.perlin;
	int depth = 7;
	fcolor col;
	if (perl->texture != NULL) {
		col = ((texture *)perl->texture)->TextureColor(perl->texture, u, v, pt, normal);
	} else {
		col = fcolor_new(1.0, 1.0, 1.0);
	}

	point3 scaled_pt = vec3_mul(pt, perl->scale);
	return color_mul(col, perlin_turbulence(perl->perlin, &pt, depth));
}

fcolor PerlinMarbledTextureColor(void *self, float u, float v, vec3 pt, vec3 *normal) {
	perlin_texture *perl = &((texture *)self)->type.perlin;
	int depth = 7;
	fcolor col;
	if (perl->texture != NULL) {
		col = ((texture *)perl->texture)->TextureColor(perl->texture, u, v, pt, normal);
	} else {
		col = fcolor_new(1.0, 1.0, 1.0);
	}
	float noise = 0.5 * (1.0 + sinf(pt.y + 10.0 * perlin_turbulence(perl->perlin, &pt, depth)));
	return color_mul(col, noise);
}

fcolor PerlinSinCosTextureColor(void *self, float u, float v, vec3 pt, vec3 *normal) {
	multicolor_perlin_texture *perl = &((texture *)self)->type.multicolor_perlin;
	int depth = 7;
	vec3 swizzled = vec3_new(pt.z, pt.x, pt.y);
	float turb = sinf(perl->scale * 10.0 * perlin_turbulence(perl->perlin, &pt, depth));
	turb *= cosf(perl->scale * perlin_turbulence(perl->perlin, &swizzled, depth));
	if (turb > 0.0) {
		return color_mul(((texture *)perl->colA)->TextureColor(perl->colA, u, v, pt, normal), turb);
	} else {
		return color_mul(((texture *)perl->colB)->TextureColor(perl->colB, u, v, pt, normal), fabs(turb));
	}
}

fcolor FBMModifierTextureColor(void *self, float u, float v, vec3 pt, vec3 *normal) {
	fbm_modifier *fbm_mod = &((texture *)self)->type.fbm_mod;
	vec3 warped = vec3_mul(pt, fbm(fbm_mod->perlin, pt, fbm_mod->hurst, fbm_mod->octaves));
	return ((texture *)fbm_mod->text)->TextureColor(fbm_mod->text, u, v, warped, normal);
}

fcolor UNDEFINED_TextureColor(void *self, float u, float v, point3 pt, vec3 *normal) {
	return COLOR_UNDEFPURP;
}

// NOTE: I think depending on NAN is bad practice but it seems like it works here
vec3 vec3_fmodf(vec3 *a, vec3 *m) {
	vec3 r = {};
	/*
	r.x = (m->x == 0.0) ? NAN : fmodf(a->x, m->x);
	r.y = (m->y == 0.0) ? NAN : fmodf(a->y, m->y);
	r.z = (m->z == 0.0) ? NAN : fmodf(a->z, m->z);
	*/

	r.x = fmodf(a->x, m->x);
	r.y = fmodf(a->y, m->y);
	r.z = fmodf(a->z, m->z);
	return r;
}

fcolor LevelCurveTextureColor(void *self, float u, float v, vec3 pt, vec3 *normal) {
	level_curve_texture lc_txt = ((texture *)self)->type.level_curve;
	vec3 abs_pt = vec3_abs(pt);
	vec3 r = vec3_fmodf(&abs_pt, &lc_txt.intervals);
	if (r.x < lc_txt.widths.x || r.y < lc_txt.widths.y || r.z < lc_txt.widths.z) return ((texture *)lc_txt.targetColor)->TextureColor(((texture *)lc_txt.targetColor), u, v, pt, normal);
	return ((texture *)lc_txt.defaultColor)->TextureColor(((texture *)lc_txt.defaultColor), u, v, pt, normal);
}

