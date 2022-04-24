#ifndef SCENE_H
#define SCENE_H

#include "memory.h"
#include "common.h"
#include "color.h"
#include "objects.h"
#include "texture.h"
#include "material.h"


typedef struct {
	object **objects;
	int object_count;
} scene;

scene RandomTestScene(memory_region *region) {
	int object_count = 10;
	object **object_list = (object **)malloc(sizeof(object *) * object_count);

	material diff = make_lambertian();
	material *matptr = (material *)memory_region_add(region, &diff, sizeof(material));

	int i;
	for(i = 0; i < object_count / 2; ++i) {
		texture *txtptr = add_color_texture(region, fcolor_new(random_float(), random_float(), random_float()));
		object o = make_sphere(vec3_new((float)i * -2.0 - 2.0,
											random_float_between(-(float)i, (float)i),
											random_float_between(-10.0, 10.0)),
										random_float() + 0.2,
										txtptr, matptr);
		object_list[i] = (object *)memory_region_add(region, &o, sizeof(object));
	}

	for(; i < object_count; ++i) {
		float anchor = random_float_between(-5.0, 5.0);
		texture *txtptr = add_color_texture(region, fcolor_new(random_float(), random_float(), random_float()));
		object o = make_triangle(
				vec3_new(random_float_between(-8, -5), anchor + random_float_between(0, 1), anchor + random_float_between(0, 4)),
				vec3_new(random_float_between(-8, -3), anchor - random_float_between(-2, 2), anchor + random_float_between(-1, 2)),
				vec3_new(random_float_between(-8, -3), anchor + random_float_between(0, 4), anchor + random_float_between(0, 4)),
											txtptr, matptr);
		object_list[i] = (object *)memory_region_add(region, &o, sizeof(object));
	}
	
	scene scene = {};
	scene.objects = object_list;
	scene.object_count = object_count;
	return scene;
}

scene BlackWhite(memory_region *region) {
	int obj_ct = 0;
	material diff = make_lambertian();
	material *matptr = (material *)memory_region_add(region, &diff, sizeof(material));

	texture *white = add_color_texture(region, COLOR_WHITE);

	object *white_sphere = add_sphere(region, vec3_new(-5.0, -1.0, 0.5), 1.0, white, matptr);
	++obj_ct;

	texture *black = add_color_texture(region, COLOR_BLACK);
	object *black_sphere = add_sphere(region, vec3_new(-5.0, 1.0, 0.5), 1.0, black, matptr);
	++obj_ct;

	texture *yellow = add_color_texture(region, fcolor_new(1.0, 0.8, 0.3));
	material *clearmetal = add_metal(region, 0.0);
	object *metal_sphere = add_sphere(region, vec3_new(-5.0, 0.0, -0.4), 0.1, yellow, clearmetal);
	++obj_ct;

	// For some reason this does not get a shadown on it... idk why.
	texture *tritext = add_color_texture(region, fcolor_new(0.3, 0.5, 0.33));
	object *base = add_triangle(region, vec3_new(20.0, 0.0, -5.0), vec3_new(-20.0, -20.0, -0.5), vec3_new(-20.0, 20.0, -0.5), tritext, matptr);
	++obj_ct;

	object **objects = (object **)malloc(obj_ct * sizeof(object));

	objects[0] = white_sphere;
	objects[1] = black_sphere;
	objects[2] = base;
	objects[3] = metal_sphere;

	scene s = {};

	s.objects = objects;
	s.object_count = obj_ct;
	return s;
}

scene BlackWhiteGlass(memory_region *region) {
	int obj_ct = 0;
	material diff = make_glass(1.51);
	material *matptr = (material *)memory_region_add(region, &diff, sizeof(material));

	texture *white = add_color_texture(region, COLOR_WHITE);

	object *white_sphere = add_sphere(region, vec3_new(-5.0, -1.0, 0.5), 1.0, white, matptr);
	++obj_ct;

	texture *black = add_color_texture(region, COLOR_BLACK);
	object *black_sphere = add_sphere(region, vec3_new(-5.0, 1.0, 0.5), 1.0, black, matptr);
	++obj_ct;

	texture *yellow = add_color_texture(region, fcolor_new(1.0, 0.8, 0.3));
	material *clearmetal = add_metal(region, 0.0);
	object *metal_sphere = add_sphere(region, vec3_new(-5.0, 0.0, -0.4), 0.1, yellow, clearmetal);
	++obj_ct;

	material *lamb = add_lambertian(region);
	texture *noisetext = add_perlin_noise_texture(region, 1.0);
	object *base = add_triangle(region, vec3_new(20.0, 0.0, -0.5), vec3_new(-20.0, -20.0, -0.5), vec3_new(-20.0, 20.0, -0.5), noisetext, lamb);
	++obj_ct;

	object **objects = (object **)malloc(obj_ct * sizeof(object));

	objects[0] = white_sphere;
	objects[1] = black_sphere;
	objects[2] = base;
	objects[3] = metal_sphere;

	scene s = {};

	s.objects = objects;
	s.object_count = obj_ct;
	return s;
}

scene RainbowCircle(memory_region *region) {
	int object_count = 8;
	int object_index = 0;
	object **object_list = (object **)malloc(sizeof(object *) * object_count);

	material *lambertian = add_lambertian(region);
	material *clearmetal = add_metal(region, 0.0);

	vec3 x_offset = vec3_new(-4.0, 0.0, 0.0);
	texture *centertxt = add_color_texture(region, fcolor_new(1.0, 1.0, 1.0));
	object *center_sphere = add_sphere(region, vec3_add(x_offset, vec3_new(0.0, 0.0, 0.0)), 1.0, centertxt, clearmetal);
	object_list[object_index] = center_sphere;
	++object_index;

	vec3 offset = vec3_new(0.0, 1.0, 0.0);

	int outer_count = 7;
	float outer_radius = 2.1;
	float r = 0.9;
	texture *rainbow[7];
	rainbow[0] = add_color_texture(region, COLOR_RED);
	rainbow[1] = add_color_texture(region, fcolor_new(0.7, 0.5, 0.0));
	rainbow[2] = add_color_texture(region, fcolor_new(0.8, 0.8, 0.0));
	rainbow[3] = add_color_texture(region, fcolor_new(0.0, 1.0, 0.0));
	rainbow[4] = add_color_texture(region, fcolor_new(0.0, 0.0, 1.0));
	rainbow[5] = add_color_texture(region, fcolor_new(0.12, 0.0, 0.6));
	rainbow[6] = add_color_texture(region, fcolor_new(0.77, 0.0, 0.77));
	float theta_inc = 360.0 / (float)(outer_count);
	for (int i = 0; i < outer_count; ++i) {
		float theta = -degrees_to_radians(theta_inc * (float)i);
		vec3 center = vec3_mul(vec3_new(0.0, cosf(theta), sinf(theta)), outer_radius);
		center = vec3_add(x_offset, center);
		object *o = add_sphere(region, center, r, rainbow[i], lambertian);
		object_list[object_index] = o;
		++object_index;
	}

	scene s = {};
	s.objects = object_list;
	s.object_count = object_index;
	return s;
}

scene TestReflection(memory_region *region) {
	int object_count = 9;
	int object_index = 0;
	object **object_list = (object **)malloc(sizeof(object *) * object_count);

	material *blurrymetal = add_metal(region, 0.2);
	material *clearmetal = add_metal(region, 0.0);

	vec3 x_offset = vec3_new(-5.0, 0.0, 0.0);

	texture *lightgray = add_color_texture(region, fcolor_new(0.8, 0.8, 0.8));

	object *center_sphere = add_sphere(region, vec3_add(x_offset, vec3_new(5.0, 0.0, -102.0)), 100.0, lightgray, clearmetal);
	object_list[object_index] = center_sphere;
	++object_index;

	int outer_count = 7;
	float outer_radius = 2.1;
	float r = 0.6;
	texture *rainbow[7];
	rainbow[0] = add_color_texture(region, COLOR_RED);
	rainbow[1] = add_color_texture(region, fcolor_new(0.7, 0.5, 0.0));
	rainbow[2] = add_color_texture(region, fcolor_new(0.8, 0.8, 0.0));
	rainbow[3] = add_color_texture(region, fcolor_new(0.0, 1.0, 0.0));
	rainbow[4] = add_color_texture(region, fcolor_new(0.0, 0.0, 1.0));
	rainbow[5] = add_color_texture(region, fcolor_new(0.12, 0.0, 0.6));
	rainbow[6] = add_color_texture(region, fcolor_new(0.77, 0.0, 0.77));
	float theta_inc = pi / (float)(outer_count-1);
	for (int i = 0; i < outer_count; ++i) {
		float theta = -theta_inc * (float)i;
		vec3 center = vec3_mul(vec3_new(sinf(theta), cosf(theta), 0.0), outer_radius);
		center = vec3_add(vec3_add(x_offset, vec3_new(0.0, 0.0, -1.0)), center);
		object *o = add_sphere(region, center, r, rainbow[i], clearmetal);
		object_list[object_index] = o;
		++object_index;
	}

	texture *gray = add_color_texture(region, fcolor_new(0.6, 0.6, 0.6));
	object *tri = add_triangle(region, vec3_new(-20.0, -20.0, -5.0), vec3_new(-20.0, 0.0, 20.0), vec3_new(-20.0, 20.0, -5.0), gray, clearmetal);
	object_list[object_index] = tri;
	++object_index;

	scene s = {};
	s.objects = object_list;
	s.object_count = object_index;
	return s;

}
/*
scene TestMetal(memory_region *region) {
	int object_count = 0;

	material *blurrymetal = add_metal(region, 0.6);
	material *clearmetal = add_metal(region, 0.0);
	material *lambertian = add_lambertian(region);

	object *metal_sphere = add_sphere(region, vec3_new(-5.0, 0.0, 1.0), 1.0, fcolor_new(0.5, 0.5, 0.5), blurrymetal);
	++object_count;

	object *ground = add_sphere(region, vec3_new(-4.0, 0.0, -100.0), 100.0, fcolor_new(0.33, 0.77, 0.33), lambertian);
	++object_count;

	object *metal_tri = add_triangle(region, vec3_new(-5.0, -5.0, 0.2), vec3_new(-7.5, 0.0, 5.0), vec3_new(-10.0, 1.0, 0.2), fcolor_new(0.77, 0.77, 0.2), clearmetal);
	++object_count;


	object **object_list = (object **)malloc(sizeof(object *) * object_count);

	object_list[0] = metal_sphere;
	object_list[1] = ground;
	object_list[2] = metal_tri;

	scene s = {};
	s.objects = object_list;
	s.object_count = object_count;
	return s;
}
scene TestTriangleReflection(memory_region *region) {
	int object_count = 4;

	object **object_list = (object **)malloc(object_count * sizeof(object));

	material *clearmetal = add_metal(region, 0.0);
	material *blurrymetal = add_metal(region, 0.9);

	object *left = add_triangle(region, vec3_new(-1.0, -8.0, 0.0), vec3_new(-8.0, 0.0, 10.0), vec3_new(-8.0, 0.0, -10.0), fcolor_new(1.0, 1.0, 0.2), clearmetal);
	object *right = add_triangle(region, vec3_new(-1.0, 8.0, 0.0), vec3_new(-8.0, 0.0, 10.0), vec3_new(-8.0, 0.0, -10.0), fcolor_new(0.2, 1.0, 1.0), clearmetal);

	object *bottom = add_triangle(region, vec3_new(-8.0, 0.0, -8.0), vec3_new(-1.0, -8.0, -8.0), vec3_new(-1.0, 8.0, -8.0), fcolor_new(1.0, 0.2, 1.0), clearmetal);

	object *center = add_sphere(region, vec3_new(-4.0, 0.0, -3.0), 0.8, fcolor_new(0.4, 0.5, 0.2), blurrymetal);

	object_list[0] = left;
	object_list[1] = right;
	object_list[2] = bottom;
	object_list[3] = center;

	scene s = {};
	s.objects = object_list;
	s.object_count = object_count;
	return s;
}

scene TestMaterial(memory_region *region) {
	int object_count = 2;
	object **object_list = (object **)malloc(object_count * sizeof(object));
	material mat = make_lambertian();
	material *matptr = (material *)memory_region_add(region, &mat, sizeof(material));
	object o = make_sphere(vec3_new(-5.0, 0.0, 0.0), 1.0, fcolor_new(1.0, 0.0, 0.0), matptr);
	object_list[0] = (object *)memory_region_add(region, &o, sizeof(object));

	object o2 = make_sphere(vec3_new(-5.0, 0.0, -101.0), 100.0, fcolor_new(0.0, 0.8, 0.0), matptr);
	object_list[1] = (object *)memory_region_add(region, &o2, sizeof(object));

	scene scene = {};
	scene.objects = object_list;
	scene.object_count = object_count;
	return scene;
}
*/

scene SimpleGlass(memory_region *region) {
	int obj_count = 7;
	int obj_index = 0;
	object **objects = (object **)malloc(sizeof(object *) * obj_count);

	texture *green = add_color_texture(region, fcolor_new(0.4, 0.77, 0.1));
	texture *white = add_color_texture(region, COLOR_WHITE);
	texture *light_blue = add_color_texture(region, fcolor_new(0.5, 0.6, 1.0));
	texture *live_laugh_love = add_image_texture(region, "../resource/livelaughlove.jpg", 3);
	texture *text = add_image_texture(region, "../resource/joycekozloff.jpg", 3);

	material *glass = add_glass(region, 1.52);
	material *water = add_glass(region, 1.333);
	material *diamond = add_glass(region, 2.417);
	material *lambertian = add_lambertian(region);
	material *blurrymetal = add_metal(region, 0.55);

	vec3 middle_line = vec3_new(-1.0, 0.0, 0.0);
	vec3 left_line = vec3_new(cosf(pi/2), sinf(pi/2), 0.0);
	vec3 right_line = vec3_new(cosf(pi/2), -sinf(pi/2), 0.0);

	object *glass_ball = add_sphere(region, vec3_mul(middle_line, 4.0), 1.0, white, glass);
	object *water_ball = add_sphere(region, vec3_mul(left_line, 4.0), 1.0, white, water);
	object *diamond_ball = add_sphere(region, vec3_mul(right_line, 4.0), 1.0, white, diamond);
	object *lll_1 = add_sphere(region, vec3_mul(middle_line, 8.0), 1.0, live_laugh_love, lambertian);
	object *lll_2 = add_sphere(region, vec3_mul(left_line, 8.0), 1.0, live_laugh_love, lambertian);
	object *lll_3 = add_sphere(region, vec3_mul(right_line, 8.0), 1.0, live_laugh_love, lambertian);

	object *bg = add_sphere(region, vec3_new(-50.0, 0.0, 0.0), 30.0, text, lambertian);

	objects[obj_index++] = glass_ball;
	objects[obj_index++] = water_ball;
	objects[obj_index++] = diamond_ball;
	//objects[obj_index++] = lll_1;
	//objects[obj_index++] = lll_2;
	//objects[obj_index++] = lll_3;
	objects[obj_index++] = bg;

	scene s = {};
	s.objects = objects;
	s.object_count = obj_index;
	return s;
}

scene TestGlass(memory_region *region) {
	int object_count = 11;
	int object_index = 0;
	object **object_list = (object **)malloc(sizeof(object *) * object_count);

	texture *rainbow[7];
	rainbow[0] = add_color_texture(region, COLOR_RED);
	rainbow[1] = add_color_texture(region, fcolor_new(0.7, 0.5, 0.0));
	rainbow[2] = add_color_texture(region, fcolor_new(0.8, 0.8, 0.0));
	rainbow[3] = add_color_texture(region, fcolor_new(0.0, 1.0, 0.0));
	rainbow[4] = add_color_texture(region, fcolor_new(0.0, 0.0, 1.0));
	rainbow[5] = add_color_texture(region, fcolor_new(0.12, 0.0, 0.6));
	rainbow[6] = add_color_texture(region, fcolor_new(0.77, 0.0, 0.77));
	texture *white = add_color_texture(region, COLOR_WHITE);
	texture *purple = add_color_texture(region, COLOR_UNDEFPURP);

	material *lambertian = add_lambertian(region);
	material *clearmetal = add_metal(region, 0.0);
	material *glass = add_glass(region, 1.52);

	vec3 x_offset = vec3_new(-15.0, 0.0, 0.0);

	object *glass_sphere = add_sphere(region, vec3_new(-5.0, 0.0, 0.0), 1.2, purple, glass);
	object_list[object_index] = glass_sphere;
	++object_index;

	object *sphere_below = add_sphere(region, vec3_new(-5.0, 0.0, -5.0), 0.8, purple, clearmetal);
	object_list[object_index] = sphere_below;
	++object_index;

	object *center_sphere = add_sphere(region, vec3_add(x_offset, vec3_new(0.0, 0.0, 0.0)), 1.0, white, clearmetal);
	object_list[object_index] = center_sphere;
	++object_index;

	vec3 offset = vec3_new(0.0, 1.0, 0.0);

	int outer_count = 7;
	float outer_radius = 2.1;
	float r = 0.9;
	float theta_inc = 360.0 / (float)(outer_count);
	for (int i = 0; i < outer_count; ++i) {
		float theta = -degrees_to_radians(theta_inc * (float)i);
		vec3 center = vec3_mul(vec3_new(0.0, cosf(theta), sinf(theta)), outer_radius);
		center = vec3_add(x_offset, center);
		object *o = add_sphere(region, center, r, rainbow[i], lambertian);
		object_list[object_index] = o;
		++object_index;
	}

	texture *gray = add_color_texture(region, fcolor_new(0.6, 0.6, 0.6));
	object *tri = add_triangle(region, vec3_new(-20.0, -20.0, -5.0), vec3_new(-20.0, 0.0, 20.0), vec3_new(-20.0, 20.0, -5.0), gray, lambertian);
	object_list[object_index] = tri;
	++object_index;

	scene s = {};
	s.objects = object_list;
	s.object_count = object_index;
	return s;
}

scene TestTextures(memory_region *region) {
	int object_count = 0;

	material *lambertian = add_lambertian(region);
	material *glass = add_glass(region, 1.52);
	material *blurrymetal = add_metal(region, 0.69);

	texture *img = add_image_texture(region, "../resource/text1.jpg", 3);
	texture *img2 = add_image_texture(region, "../resource/earth.jpg", 3);
	object *tri = add_triangle(region, vec3_new(-5.0, 0.0, 5.0), vec3_new(-5.0, -5.0, 0.0), vec3_new(-5.0, 5.0, 0.0), img2, lambertian); 
	++object_count;

	texture *uvtxt = add_uv_texture(region);
	object *sph = add_sphere(region, vec3_new(-8.0, 0.0, -2.0), 1.5, img2, lambertian);
	++object_count;
	object *sph2 = add_sphere(region, vec3_new(-8.0, -3.0, -2.0), 1.5, img, blurrymetal);
	++object_count;
	object *sph3 = add_sphere(region, vec3_new(-8.0, 3.0, -2.0), 1.5, img2, glass);
	++object_count;

	object **objects = (object **)malloc(sizeof(object *) * object_count);
	objects[0] = tri;
	objects[1] = sph;
	objects[2] = sph2;
	objects[3] = sph3;

	scene s = {};
	s.objects = objects;
	s.object_count = object_count;
	return s;
}

scene TestGlassAndTextures(memory_region *region) {
	int object_count = 10;
	int object_index = 0;
	object **object_list = (object **)malloc(sizeof(object *) * object_count);

	material *lambertian = add_lambertian(region);
	material *clearmetal = add_metal(region, 0.0);
	material *glass = add_glass(region, 1.52);

	vec3 x_offset = vec3_new(-15.0, 0.0, 0.0);

	texture *white = add_color_texture(region, COLOR_WHITE);
	texture *earth = add_image_texture(region, "../resource/earth.jpg", 3);

	object *glass_sphere = add_sphere(region, vec3_new(-5.0, 0.0, 0.0), 1.2, white, glass);
	object_list[object_index] = glass_sphere;
	++object_index;

	object *center_sphere = add_sphere(region, vec3_add(x_offset, vec3_new(0.0, 0.0, 0.0)), 1.0, earth, clearmetal);
	object_list[object_index] = center_sphere;
	++object_index;

	vec3 offset = vec3_new(0.0, 1.0, 0.0);

	int outer_count = 7;
	float outer_radius = 2.1;
	float r = 0.9;
	texture *rainbow[7];
	rainbow[0] = add_color_texture(region, COLOR_RED);
	rainbow[1] = add_color_texture(region, fcolor_new(0.7, 0.5, 0.0));
	rainbow[2] = add_color_texture(region, fcolor_new(0.8, 0.8, 0.0));
	rainbow[3] = add_color_texture(region, fcolor_new(0.0, 1.0, 0.0));
	rainbow[4] = add_color_texture(region, fcolor_new(0.0, 0.0, 1.0));
	rainbow[5] = add_color_texture(region, fcolor_new(0.12, 0.0, 0.6));
	rainbow[6] = add_color_texture(region, fcolor_new(0.77, 0.0, 0.77));
	float theta_inc = 360.0 / (float)(outer_count);
	for (int i = 0; i < outer_count; ++i) {
		float theta = -degrees_to_radians(theta_inc * (float)i);
		vec3 center = vec3_mul(vec3_new(0.0, cosf(theta), sinf(theta)), outer_radius);
		center = vec3_add(x_offset, center);
		object *o = add_sphere(region, center, r, rainbow[i], lambertian);
		object_list[object_index] = o;
		++object_index;
	}

	texture *gray = add_color_texture(region, fcolor_new(0.6, 0.6, 0.6));
	object *tri = add_triangle(region, vec3_new(-20.0, -20.0, -5.0), vec3_new(-20.0, 0.0, 20.0), vec3_new(-20.0, 20.0, -5.0), gray, lambertian);
	object_list[object_index] = tri;
	++object_index;

	scene s = {};
	s.objects = object_list;
	s.object_count = object_index;
	return s;
}

scene MiscTextureTest(memory_region *region) {
	int max_obj = 10;
	int obj_ct = 0;
	object **objects = (object **)malloc(max_obj * sizeof(object));

	material *lamb = add_lambertian(region);
	material *clearmetal = add_metal(region, 0.0);

	texture *white = add_color_texture(region, COLOR_WHITE);
	texture *yellow = add_color_texture(region, fcolor_new(1.0, 0.8, 0.3));
	texture *black = add_color_texture(region, COLOR_BLACK);
	texture *gray = add_color_texture(region, COLOR_VALUE(0.68));
	texture *purple = add_color_texture(region, COLOR_UNDEFPURP);
	texture *checker = add_checker_texture(region, black, white, 1.0);
	texture *uvchecker = add_uv_checker_texture(region, white, gray, 100.0);
	texture *perlin = add_colored_perlin_noise_texture(region, 2.0, uvchecker);
	texture *perlin2 = add_colored_perlin_turbulence_texture(region, 5.0, uvchecker);

	object *ls = add_sphere(region, vec3_new(0.0, -1.0, 1.0), 1.0, perlin, lamb);
	object *rs = add_sphere(region, vec3_new(0.0, 1.0, 1.0), 1.0, perlin2, lamb);
	objects[obj_ct++] = ls;
	objects[obj_ct++] = rs;
	vec3 a = vec3_new(20.0, 0.0, 0.0);
	vec3 b = vec3_new(-20.0, 0.0, 0.0);
	vec3 c = vec3_new(0.0, -20.0, 0.0);
	vec3 d = vec3_new(0.0, 20.0, 0.0);
	object *abd = add_triangle(region, a, b, d, perlin, lamb);
	object *abc = add_triangle(region, a, b, c, perlin2, lamb);
	objects[obj_ct++] = abc;
	objects[obj_ct++] = abd;

	scene s = {};

	s.objects = objects;
	s.object_count = obj_ct;
	return s;
}

scene SceneSelect(memory_region *region, int selection) {
	switch(selection) {
		case 1:
			return BlackWhite(region);
		case 2:
			return RainbowCircle(region);
		case 3:
			return TestReflection(region);
		case 4:
			return TestGlass(region);
		case 5:
			return TestTextures(region);
		case 6:
			return TestGlassAndTextures(region);
		case 7:
			return BlackWhiteGlass(region);
		case 8:
			return SimpleGlass(region);
		case 9:
			return MiscTextureTest(region);
		default:
			return RandomTestScene(region);

	}
}

void FreeScene(scene *scene) {
	free(scene->objects);
}

#endif
