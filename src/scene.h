#ifndef SCENE_H
#define SCENE_H

#include "common.h"
#include "memory.h"
#include "color.h"
#include "objects.h"
#include "texture.h"
#include "material.h"

typedef struct {
	object **objects;
	int object_count;
} scene;

scene Demo(memory_region *region, scene *s, vec3 *origin, vec3 *target, thread_context *thread) {
	int max_object_count = 64;
	int index = 0;

	*origin = vec3_new(5.0f, 0.0f, 0.0f);
	*target = vec3_new(-2.0f, 0.0f, 0.0f);

	object **obj_list = (object **)malloc(sizeof(object *) * max_object_count);

	material *glass = add_glass(region, 1.52f);
	material *steel = add_metal(region, 0.0f);
	material *iron = add_metal(region, 0.65f);
	material *lambertian = add_lambertian(region);
	material *light = add_diffuse_light(region);

	texture *white_light = add_color_texture(region, COLOR_VALUE(7.0f));

	texture *white = add_color_texture(region, COLOR_WHITE);
	texture *lightgray = add_color_texture(region, COLOR_VALUE(0.8f));
	texture *darkgray = add_color_texture(region, COLOR_VALUE(0.3f));
	texture *black = add_color_texture(region, COLOR_BLACK);

	texture *ivory = add_color_texture(region, fcolor_new(0.8f, 0.8f, 0.75f));
	texture *dullgreen = add_color_texture(region, fcolor_new(0.3f, 0.66f, 0.56f));

	texture *purple = add_color_texture(region, fcolor_new(0.7f, 0.2f, 0.65f));
	texture *red = add_color_texture(region, COLOR_RED);

	texture *marbled = add_marbled_noise_texture(region, NULL, 16.0f, purple);
	texture *trippy = add_noise_sincos_texture(region, NULL, 2.0f, purple, darkgray);

	texture *checkerboard = add_checker_texture(region, darkgray, white, 2.0f);
	texture *chessboard = add_checker_texture(region, ivory, dullgreen, 0.5f);
	texture *checkception = add_checker_texture(region, checkerboard, chessboard, 1.0f);


	object *glassball = add_sphere(region, vec3_new(-2.0f, 0.0f, 1.0f), 1.0f, white, glass);
	object *base = add_sphere(region, vec3_new(-5.0f, 0.0f, -100.0f), 100.0f, trippy, lambertian);
	object *leftball = add_sphere(region, vec3_new(-10.0f, -4, 1.0f), 0.8f, white_light, light);
	float trifloat = 500.0f;
	object *back = add_triangle(region, vec3_new(-50.0f, 0.0f, -trifloat), vec3_new(-50.0f, -trifloat, trifloat), vec3_new(-50.0f, trifloat, trifloat), chessboard, lambertian);

	obj_list[index++] = glassball;
	obj_list[index++] = base;
	obj_list[index++] = leftball;
	obj_list[index++] = back;

	scene new_scene = {};
	new_scene.objects = obj_list;
	new_scene.object_count = index;
	*s = new_scene;
	return new_scene;
}

scene TestLight(memory_region *region, scene *s, vec3 *origin, vec3 *target) {
	int max_object_count = 64;
	int index = 0;

	*origin = vec3_new(5.0f, 0.0f, 1.0f);
	*target = vec3_new(-2.0f, 0.0f, 0.0f);

	object **obj_list = (object **)malloc(sizeof(object *) * max_object_count);

	material *glass = add_glass(region, 1.52f);
	material *steel = add_metal(region, 0.2f);
	material *iron = add_metal(region, 0.8f);
	material *lambertian = add_lambertian(region);
	material *light = add_diffuse_light(region);

	texture *white_light = add_color_texture(region, COLOR_VALUE(7.0f));
	texture *red_light = add_color_texture(region, fcolor_new(5.0f, 0.0f, 0.0f));
	texture *green_light = add_color_texture(region, fcolor_new(0.0f, 5.0f, 0.0f));
	texture *blue_light = add_color_texture(region, fcolor_new(0.0f, 0.0f, 5.0f));

	texture *white = add_color_texture(region, COLOR_WHITE);
	texture *lightgray = add_color_texture(region, COLOR_VALUE(0.8f));
	texture *darkgray = add_color_texture(region, COLOR_VALUE(0.3f));
	texture *black = add_color_texture(region, COLOR_VALUE(0.001f));

	texture *ivory = add_color_texture(region, fcolor_new(0.8f, 0.8f, 0.75f));
	texture *dullgreen = add_color_texture(region, fcolor_new(0.3f, 0.66f, 0.56f));

	texture *purple = add_color_texture(region, fcolor_new(0.7f, 0.2f, 0.65f));
	texture *red = add_color_texture(region, COLOR_RED);

	texture *noisetext = add_noise_texture(region, NULL, 4.0f);
	texture *marbled = add_marbled_noise_texture(region, NULL, 16.0f, white);
	texture *trippy = add_noise_sincos_texture(region, NULL, 2.0f, black, white);
	texture *trippy_light = add_noise_sincos_texture(region, NULL, 2.0f, green_light, black);

	object *glassball = add_sphere(region, vec3_new(-2.0f, 0.0f, 0.5f), 0.5f, white, glass);
	// Left side - Red
	float divider = 2.0f;
	object *left_metalball = add_sphere(region, vec3_new(-6.0f, -(2.0f + divider), 2.0f), 2.0f, white, iron);
	object *sincosball = add_sphere(region, vec3_new(-6.0f, -2.0f - divider, 2.0f), 2.0f, trippy, lambertian);
	vec3 lighta =vec3_new(-6.0f, -5.0f - divider, 4.0f);
	vec3 lightb = vec3_new(-4.0f, -5.5f - divider, 2.0f);
	vec3 lightc = vec3_new(-8.0f, -4.5f - divider, 2.0f);
	object *r_sphere = add_sphere(region, vec3_new(-4.0f, -(4.0f + divider), 4.0f), 1.0f, red_light, light);

	//object *trilight = add_triangle(region, lightb, lighta, lightc, red_light, light);
	
	// Middle - Green
	object *g_sphere = add_sphere(region, vec3_new(-6.0f, 0.0f, 2.0f), 1.0f, green_light, light);

	// Right side - Blue
	object *metalball = add_sphere(region, vec3_new(-6.0f, 2.0f + divider, 2.0f), 2.0f, white, iron);
	object *perlinball = add_sphere(region, vec3_new(-6.0f, 2.0f + divider, 2.0f), 2.0f, marbled, lambertian);
	object *lightball = add_sphere(region, vec3_new(-4.0f, 4.0f + divider, 4.0f), 1.0f, blue_light, light);
	float trifloat = 100.0f;
	vec3 floor_a = vec3_new(trifloat, 0.0f, 0.0f);
	vec3 floor_b = vec3_new(-trifloat, -2.0f*trifloat, 0.0f);
	vec3 floor_c = vec3_new(-trifloat, 2.0f * trifloat, 0.0f);
	object *floor = add_triangle(region, floor_a, floor_b, floor_c, noisetext, lambertian);

	vec3 back_a = vec3_new(-trifloat, -trifloat, 0.0f);
	vec3 back_b = vec3_new(-trifloat, trifloat, 0.0f);
	vec3 back_c = vec3_new(-trifloat, 0.0f, trifloat);
	object *back = add_triangle(region, back_a, back_b, back_c, white_light, light);

	obj_list[index++] = glassball;
	obj_list[index++] = g_sphere;

	obj_list[index++] = sincosball;
	obj_list[index++] = r_sphere;
	obj_list[index++] = perlinball;
	//obj_list[index++] = left_metalball;
	//obj_list[index++] = metalball;
	obj_list[index++] = lightball;
	obj_list[index++] = floor;

	scene new_scene = {};
	new_scene.objects = obj_list;
	new_scene.object_count = index;
	*s = new_scene;
	return new_scene;
}

scene ScuffedCornellBox(memory_region *region, scene *s, vec3 *origin, vec3 *target, thread_context *thread) {
	int max_object_count = 64;
	int index = 0;

	*origin = vec3_new(5.5f, 0.0f, 6.0f);
	*target = vec3_new(0.0f, 0.0f, 6.0f);

	object **obj_list = (object **)malloc(sizeof(object *) * max_object_count);

	material *lambertian = add_lambertian(region);
	material *light = add_diffuse_light(region);
	material *steel = add_metal(region, 0.15f);
	material *glass = add_glass(region, 1.52f);

	noise *shared_perlin = add_perlin_noise(region, 64, thread);
	noise *shared_simp = add_simplex_noise(region, 64, thread);

	texture *white_light = add_color_texture(region, COLOR_VALUE(10.0f));
	texture *white = add_color_texture(region, COLOR_WHITE);
	texture *gray = add_color_texture(region, COLOR_VALUE(0.5f));
	texture *black = add_color_texture(region, COLOR_VALUE(0.001f));

	texture *gold = add_color_texture(region, fcolor_new(0.6f, 0.6f, 0.2f));

	texture *noisetext = add_noise_texture(region, shared_simp, 4.0f);

	texture *ivory = add_color_texture(region, fcolor_new(0.8f, 0.8f, 0.75f));
	texture *dullgreen = add_color_texture(region, fcolor_new(0.3f, 0.66f, 0.56f));

	texture *purple = add_color_texture(region, fcolor_new(0.7f, 0.2f, 0.65f));
	texture *red = add_color_texture(region, COLOR_RED);

	texture *checkerboard = add_checker_texture(region, red, black, 2.0f);
	texture *chessboard = add_checker_texture(region, ivory, dullgreen, 2.0f);

	texture *trippy = add_noise_sincos_texture(region, shared_simp, 8.0f, gold, dullgreen);
	texture *trippy2 = add_noise_sincos_texture(region, shared_perlin, 8.0f, gold, dullgreen);

	object *fbm_src = add_sphere(region, vec3_new(-7.0f, 3.0f, 1.5f), 1.5f, ivory, lambertian);
	object *fbm_perlinball = add_fbm_sphere(region, shared_simp, 0.2f, 0.2f, 0.5f, 6, fbm_src);
	object *goldball = add_sphere(region, vec3_new(-6.0f, -3.0f, 1.0f), 1.0f, gold, steel);
	object *glassball = add_sphere(region, vec3_new(-4.0f, 0.0f, 1.0f), 1.0f, noisetext, lambertian);

	float z = 12.0f;
	vec3 pt_a = vec3_new(-8.0f, -3.0f,z);
	vec3 pt_b = vec3_new(-5.0f, -3.0f, z);
	vec3 pt_c = vec3_new(-8.0f, 3.0f, z);
	vec3 pt_d = vec3_new(-5.0f, 3.0f, z);
	object *light_tri = add_single_sided_triangle(region, pt_b, pt_a, pt_d, white_light, light);
	object *light_tri2 = add_single_sided_triangle(region, pt_a, pt_c, pt_d, white_light, light);

	float trifloat = 100.0f;
	vec3 floor_a = vec3_new(trifloat, 0.0f, 0.0f);
	vec3 floor_b = vec3_new(-trifloat, -2.0f*trifloat, 0.0f);
	vec3 floor_c = vec3_new(-trifloat, 2.0f * trifloat, 0.0f);
	object *floor = add_triangle(region, floor_a, floor_b, floor_c, gray, lambertian);

	float x = -10.0f;
	vec3 back_a = vec3_new(x, -6.0f, 0.0f);
	vec3 back_b = vec3_new(x, 6.0f, 0.0f);
	vec3 back_c = vec3_new(x, -6.0f, 20.0f);
	vec3 back_d = vec3_new(x, 6.0f, 20.0f);
	object *back = add_triangle(region, back_a, back_b, back_c, gray, steel);
	object *back2 = add_triangle(region, back_d, back_b, back_c, gray, steel);

	float ly = -6.0f;
	vec3 left_a = vec3_new(x,ly,0.0f);
	vec3 left_b = vec3_new(x,ly,20.0f);
	vec3 left_c = vec3_new(0.0f,ly,0.0f);
	vec3 left_d = vec3_new(0.0f,ly,20.0f);
	object *left = add_triangle(region, left_d, left_b, left_c, gray, steel);
	object *left2 = add_triangle(region, left_a, left_b, left_c, gray, steel);

	float ry = 6.0f;
	vec3 right_a = vec3_new(x,ry,0.0f);
	vec3 right_b = vec3_new(x,ry,20.0f);
	vec3 right_c = vec3_new(0.0f,ry,0.0f);
	vec3 right_d = vec3_new(0.0f,ry,20.0f);
	object *right = add_triangle(region, right_d, right_b, right_c, trippy, lambertian);
	object *right2 = add_triangle(region, right_a, right_b, right_c, trippy2, lambertian);


	obj_list[index++] = glassball;
	obj_list[index++] = fbm_perlinball;
	obj_list[index++] = goldball;
	obj_list[index++] = light_tri;
	obj_list[index++] = light_tri2;
	obj_list[index++] = floor;
	obj_list[index++] = back;
	obj_list[index++] = back2;
	obj_list[index++] = left;
	obj_list[index++] = left2;
	obj_list[index++] = right;
	obj_list[index++] = right2;

	scene new_scene = {};
	new_scene.objects = obj_list;
	new_scene.object_count = index;
	*s = new_scene;
	return new_scene;

}

scene TestTriangleLight(memory_region *region, scene *s, vec3 *origin, vec3 *target) {
	int max_object_count = 64;
	int index = 0;

	*origin = vec3_new(5.0f, 4.0f, 1.0f);
	*target = vec3_new(-2.0f, 0.0f, 0.0f);

	object **obj_list = (object **)malloc(sizeof(object *) * max_object_count);

	material *lambertian = add_lambertian(region);
	material *light = add_diffuse_light(region);

	texture *white_light = add_color_texture(region, COLOR_VALUE(7.0f));

	texture *white = add_color_texture(region, COLOR_WHITE);
	texture *black = add_color_texture(region, COLOR_VALUE(0.001f));

	texture *noisetext = add_noise_texture(region, NULL, 4.0f);
	texture *trippy = add_noise_sincos_texture(region, NULL, 2.0f, black, white);

	object *perlinball = add_sphere(region, vec3_new(-6.0f, 0.0f, 2.0f), 2.0f, trippy, lambertian);

	vec3 pt_a = vec3_new(-6.0f, 3.0f, 4.5f);
	vec3 pt_b = vec3_new(-5.0f, 3.0f, 0.5f);
	vec3 pt_c = vec3_new(-7.0f, 3.0f, 0.5f);
	object *light_tri = add_single_sided_triangle(region, pt_b, pt_a, pt_c, white_light, light);

	float trifloat = 100.0f;
	vec3 floor_a = vec3_new(trifloat, 0.0f, 0.0f);
	vec3 floor_b = vec3_new(-trifloat, -2.0f*trifloat, 0.0f);
	vec3 floor_c = vec3_new(-trifloat, 2.0f * trifloat, 0.0f);
	object *floor = add_triangle(region, floor_a, floor_b, floor_c, noisetext, lambertian);

	obj_list[index++] = perlinball;
	obj_list[index++] = light_tri;
	obj_list[index++] = floor;

	scene new_scene = {};
	new_scene.objects = obj_list;
	new_scene.object_count = index;
	*s = new_scene;
	return new_scene;
}

/*
scene RandomTestScene(memory_region *region) {
	int object_count = 10;
	object **object_list = (object **)malloc(sizeof(object *) * object_count);

	material diff = make_lambertian();
	material *matptr = (material *)memory_region_add(region, &diff, sizeof(material));

	int i;
	for(i = 0; i < object_count / 2; ++i) {
		texture *txtptr = add_color_texture(region, fcolor_new(random_float(), random_float(), random_float()));
		object o = make_sphere(vec3_new((float)i * -2.0f - 2.0f,
											random_float_between(-(float)i, (float)i),
											random_float_between(-10.0f, 10.0f)),
										random_float() + 0.2f,
										txtptr, matptr);
		object_list[i] = (object *)memory_region_add(region, &o, sizeof(object));
	}

	for(; i < object_count; ++i) {
		float anchor = random_float_between(-5.0f, 5.0f);
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
*/

scene BlackWhite(memory_region *region) {
	int obj_ct = 0;
	material diff = make_lambertian();
	material *matptr = (material *)memory_region_add(region, &diff, sizeof(material));

	texture *white = add_color_texture(region, COLOR_WHITE);

	object *white_sphere = add_sphere(region, vec3_new(-5.0f, -1.0f, 0.5f), 1.0f, white, matptr);
	++obj_ct;

	texture *black = add_color_texture(region, COLOR_BLACK);
	object *black_sphere = add_sphere(region, vec3_new(-5.0f, 1.0f, 0.5f), 1.0f, black, matptr);
	++obj_ct;

	texture *yellow = add_color_texture(region, fcolor_new(1.0f, 0.8f, 0.3f));
	material *clearmetal = add_metal(region, 0.0f);
	object *metal_sphere = add_sphere(region, vec3_new(-5.0f, 0.0f, -0.4f), 0.1f, yellow, clearmetal);
	++obj_ct;

	// For some reason this does not get a shadown on it... idk why.
	texture *tritext = add_color_texture(region, fcolor_new(0.3f, 0.5f, 0.33f));
	object *base = add_triangle(region, vec3_new(20.0f, 0.0f, -5.0f), vec3_new(-20.0f, -20.0f, -0.5f), vec3_new(-20.0f, 20.0f, -0.5f), tritext, matptr);
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
	material diff = make_glass(1.51f);
	material *matptr = (material *)memory_region_add(region, &diff, sizeof(material));

	texture *white = add_color_texture(region, COLOR_WHITE);

	object *white_sphere = add_sphere(region, vec3_new(-5.0f, -1.0f, 0.5f), 1.0f, white, matptr);
	++obj_ct;

	texture *black = add_color_texture(region, COLOR_BLACK);
	object *black_sphere = add_sphere(region, vec3_new(-5.0f, 1.0f, 0.5f), 1.0f, black, matptr);
	++obj_ct;

	texture *yellow = add_color_texture(region, fcolor_new(1.0f, 0.8f, 0.3f));
	material *clearmetal = add_metal(region, 0.0f);
	object *metal_sphere = add_sphere(region, vec3_new(-5.0f, 0.0f, -0.4f), 0.1f, yellow, clearmetal);
	++obj_ct;

	material *lamb = add_lambertian(region);
	texture *noisetext = add_noise_texture(region, NULL, 1.0f);
	object *base = add_triangle(region, vec3_new(20.0f, 0.0f, -0.5f), vec3_new(-20.0f, -20.0f, -0.5f), vec3_new(-20.0f, 20.0f, -0.5f), noisetext, lamb);
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

void RainbowCircle(memory_region *region, scene *s, vec3 *o, vec3 *t, thread_context *thread) {
	int object_count = 8;
	int object_index = 0;
	object **object_list = (object **)malloc(sizeof(object *) * object_count);

	material *lambertian = add_lambertian(region);
	material *clearmetal = add_metal(region, 0.0f);
	material *difflight = add_diffuse_light(region);

	vec3 x_offset = vec3_new(-4.0f, 0.0f, 0.0f);
	texture *centertxt = add_color_texture(region, fcolor_new(1.0f, 1.0f, 1.0f));
	object *center_sphere = add_sphere(region, vec3_add(x_offset, vec3_new(0.0f, 0.0f, 0.0f)), 1.0f, centertxt, clearmetal);
	object_list[object_index] = center_sphere;
	++object_index;

	vec3 offset = vec3_new(0.0f, 1.0f, 0.0f);

	int outer_count = 7;
	float outer_radius = 2.1f;
	float r = 0.9f;
	texture *white_light = add_color_texture(region, COLOR_VALUE(7.0f));
	texture *rainbow[7];
	rainbow[0] = add_color_texture(region, COLOR_RED);
	rainbow[1] = add_color_texture(region, fcolor_new(0.7f, 0.5f, 0.0f));
	rainbow[2] = add_color_texture(region, fcolor_new(0.8f, 0.8f, 0.0f));
	rainbow[3] = add_color_texture(region, fcolor_new(0.0f, 1.0f, 0.0f));
	rainbow[4] = add_color_texture(region, fcolor_new(0.0f, 0.0f, 1.0f));
	rainbow[5] = add_color_texture(region, fcolor_new(0.12f, 0.0f, 0.6f));
	rainbow[6] = add_color_texture(region, fcolor_new(0.77f, 0.0f, 0.77f));
	float theta_inc = 360.0f / (float)(outer_count);
	for (int i = 0; i < outer_count; ++i) {
		float theta = -degrees_to_radians(theta_inc * (float)i);
		vec3 center = vec3_mul(vec3_new(0.0f, cosf(theta), sinf(theta)), outer_radius);
		center = vec3_add(x_offset, center);
		object *o = add_sphere(region, center, r, rainbow[i], lambertian);
		object_list[object_index] = o;
		++object_index;
	}

	object *light = add_sphere(region, vec3_new(10.0f, 0.0f, 10.0f), 3.0f, white_light, difflight);
	object_list[object_index++] = light;

	*o = vec3_new(5.0f, 0.0f, 0.0f);
	*t = x_offset;

	s->objects = object_list;
	s->object_count = object_index;
}

scene TestReflection(memory_region *region, thread_context *thread) {
	int object_count = 9;
	int object_index = 0;
	object **object_list = (object **)malloc(sizeof(object *) * object_count);

	material *blurrymetal = add_metal(region, 0.2f);
	material *clearmetal = add_metal(region, 0.0f);

	vec3 x_offset = vec3_new(-5.0f, 0.0f, 0.0f);

	texture *lightgray = add_color_texture(region, fcolor_new(0.8f, 0.8f, 0.8f));

	object *center_sphere = add_sphere(region, vec3_add(x_offset, vec3_new(5.0f, 0.0f, -102.0f)), 100.0f, lightgray, clearmetal);
	object_list[object_index] = center_sphere;
	++object_index;

	int outer_count = 7;
	float outer_radius = 2.1f;
	float r = 0.6f;
	texture *rainbow[7];
	rainbow[0] = add_color_texture(region, COLOR_RED);
	rainbow[1] = add_color_texture(region, fcolor_new(0.7f, 0.5f, 0.0f));
	rainbow[2] = add_color_texture(region, fcolor_new(0.8f, 0.8f, 0.0f));
	rainbow[3] = add_color_texture(region, fcolor_new(0.0f, 1.0f, 0.0f));
	rainbow[4] = add_color_texture(region, fcolor_new(0.0f, 0.0f, 1.0f));
	rainbow[5] = add_color_texture(region, fcolor_new(0.12f, 0.0f, 0.6f));
	rainbow[6] = add_color_texture(region, fcolor_new(0.77f, 0.0f, 0.77f));
	float theta_inc = pi / (float)(outer_count-1);
	for (int i = 0; i < outer_count; ++i) {
		float theta = -theta_inc * (float)i;
		vec3 center = vec3_mul(vec3_new(sinf(theta), cosf(theta), 0.0f), outer_radius);
		center = vec3_add(vec3_add(x_offset, vec3_new(0.0f, 0.0f, -1.0f)), center);
		object *o = add_sphere(region, center, r, rainbow[i], clearmetal);
		object_list[object_index] = o;
		++object_index;
	}

	texture *gray = add_color_texture(region, fcolor_new(0.6f, 0.6f, 0.6f));
	object *tri = add_triangle(region, vec3_new(-20.0f, -20.0f, -5.0f), vec3_new(-20.0f, 0.0f, 20.0f), vec3_new(-20.0f, 20.0f, -5.0f), gray, clearmetal);
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

	material *blurrymetal = add_metal(region, 0.6f);
	material *clearmetal = add_metal(region, 0.0f);
	material *lambertian = add_lambertian(region);

	object *metal_sphere = add_sphere(region, vec3_new(-5.0f, 0.0f, 1.0f), 1.0f, fcolor_new(0.5f, 0.5f, 0.5f), blurrymetal);
	++object_count;

	object *ground = add_sphere(region, vec3_new(-4.0f, 0.0f, -100.0f), 100.0f, fcolor_new(0.33f, 0.77f, 0.33f), lambertian);
	++object_count;

	object *metal_tri = add_triangle(region, vec3_new(-5.0f, -5.0f, 0.2f), vec3_new(-7.5f, 0.0f, 5.0f), vec3_new(-10.0f, 1.0f, 0.2f), fcolor_new(0.77f, 0.77f, 0.2f), clearmetal);
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

	material *clearmetal = add_metal(region, 0.0f);
	material *blurrymetal = add_metal(region, 0.9f);

	object *left = add_triangle(region, vec3_new(-1.0f, -8.0f, 0.0f), vec3_new(-8.0f, 0.0f, 10.0f), vec3_new(-8.0f, 0.0f, -10.0f), fcolor_new(1.0f, 1.0f, 0.2f), clearmetal);
	object *right = add_triangle(region, vec3_new(-1.0f, 8.0f, 0.0f), vec3_new(-8.0f, 0.0f, 10.0f), vec3_new(-8.0f, 0.0f, -10.0f), fcolor_new(0.2f, 1.0f, 1.0f), clearmetal);

	object *bottom = add_triangle(region, vec3_new(-8.0f, 0.0f, -8.0f), vec3_new(-1.0f, -8.0f, -8.0f), vec3_new(-1.0f, 8.0f, -8.0f), fcolor_new(1.0f, 0.2f, 1.0f), clearmetal);

	object *center = add_sphere(region, vec3_new(-4.0f, 0.0f, -3.0f), 0.8f, fcolor_new(0.4f, 0.5f, 0.2f), blurrymetal);

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
	object o = make_sphere(vec3_new(-5.0f, 0.0f, 0.0f), 1.0f, fcolor_new(1.0f, 0.0f, 0.0f), matptr);
	object_list[0] = (object *)memory_region_add(region, &o, sizeof(object));

	object o2 = make_sphere(vec3_new(-5.0f, 0.0f, -101.0f), 100.0f, fcolor_new(0.0f, 0.8f, 0.0f), matptr);
	object_list[1] = (object *)memory_region_add(region, &o2, sizeof(object));

	scene scene = {};
	scene.objects = object_list;
	scene.object_count = object_count;
	return scene;
}
*/

scene SimpleGlass(memory_region *region, thread_context *thread) {
	int obj_count = 7;
	int obj_index = 0;
	object **objects = (object **)malloc(sizeof(object *) * obj_count);

	texture *green = add_color_texture(region, fcolor_new(0.4f, 0.77f, 0.1f));
	texture *white = add_color_texture(region, COLOR_WHITE);
	texture *light_blue = add_color_texture(region, fcolor_new(0.5f, 0.6f, 1.0f));
	texture *live_laugh_love = add_image_texture(region, "../resource/livelaughlove.jpg", 3);
	texture *text = add_image_texture(region, "../resource/joycekozloff.jpg", 3);

	material *glass = add_glass(region, 1.52f);
	material *water = add_glass(region, 1.333f);
	material *diamond = add_glass(region, 2.417f);
	material *lambertian = add_lambertian(region);
	material *blurrymetal = add_metal(region, 0.55f);

	vec3 middle_line = vec3_new(-1.0f, 0.0f, 0.0f);
	vec3 left_line = vec3_new(cosf(pi/2), sinf(pi/2), 0.0f);
	vec3 right_line = vec3_new(cosf(pi/2), -sinf(pi/2), 0.0f);

	object *glass_ball = add_sphere(region, vec3_mul(middle_line, 4.0f), 1.0f, white, glass);
	object *water_ball = add_sphere(region, vec3_mul(left_line, 4.0f), 1.0f, white, water);
	object *diamond_ball = add_sphere(region, vec3_mul(right_line, 4.0f), 1.0f, white, diamond);
	object *lll_1 = add_sphere(region, vec3_mul(middle_line, 8.0f), 1.0f, live_laugh_love, lambertian);
	object *lll_2 = add_sphere(region, vec3_mul(left_line, 8.0f), 1.0f, live_laugh_love, lambertian);
	object *lll_3 = add_sphere(region, vec3_mul(right_line, 8.0f), 1.0f, live_laugh_love, lambertian);

	object *bg = add_sphere(region, vec3_new(-50.0f, 0.0f, 0.0f), 30.0f, text, lambertian);

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

scene TestGlass(memory_region *region, thread_context *thread) {
	int object_count = 11;
	int object_index = 0;
	object **object_list = (object **)malloc(sizeof(object *) * object_count);

	texture *rainbow[7];
	rainbow[0] = add_color_texture(region, COLOR_RED);
	rainbow[1] = add_color_texture(region, fcolor_new(0.7f, 0.5f, 0.0f));
	rainbow[2] = add_color_texture(region, fcolor_new(0.8f, 0.8f, 0.0f));
	rainbow[3] = add_color_texture(region, fcolor_new(0.0f, 1.0f, 0.0f));
	rainbow[4] = add_color_texture(region, fcolor_new(0.0f, 0.0f, 1.0f));
	rainbow[5] = add_color_texture(region, fcolor_new(0.12f, 0.0f, 0.6f));
	rainbow[6] = add_color_texture(region, fcolor_new(0.77f, 0.0f, 0.77f));
	texture *white = add_color_texture(region, COLOR_WHITE);
	texture *purple = add_color_texture(region, COLOR_UNDEFPURP);

	material *lambertian = add_lambertian(region);
	material *clearmetal = add_metal(region, 0.0f);
	material *glass = add_glass(region, 1.52f);

	vec3 x_offset = vec3_new(-15.0f, 0.0f, 0.0f);

	object *glass_sphere = add_sphere(region, vec3_new(-5.0f, 0.0f, 0.0f), 1.2f, purple, glass);
	object_list[object_index] = glass_sphere;
	++object_index;

	object *sphere_below = add_sphere(region, vec3_new(-5.0f, 0.0f, -5.0f), 0.8f, purple, clearmetal);
	object_list[object_index] = sphere_below;
	++object_index;

	object *center_sphere = add_sphere(region, vec3_add(x_offset, vec3_new(0.0f, 0.0f, 0.0f)), 1.0f, white, clearmetal);
	object_list[object_index] = center_sphere;
	++object_index;

	vec3 offset = vec3_new(0.0f, 1.0f, 0.0f);

	int outer_count = 7;
	float outer_radius = 2.1f;
	float r = 0.9f;
	float theta_inc = 360.0f / (float)(outer_count);
	for (int i = 0; i < outer_count; ++i) {
		float theta = -degrees_to_radians(theta_inc * (float)i);
		vec3 center = vec3_mul(vec3_new(0.0f, cosf(theta), sinf(theta)), outer_radius);
		center = vec3_add(x_offset, center);
		object *o = add_sphere(region, center, r, rainbow[i], lambertian);
		object_list[object_index] = o;
		++object_index;
	}

	texture *gray = add_color_texture(region, fcolor_new(0.6f, 0.6f, 0.6f));
	object *tri = add_triangle(region, vec3_new(-20.0f, -20.0f, -5.0f), vec3_new(-20.0f, 0.0f, 20.0f), vec3_new(-20.0f, 20.0f, -5.0f), gray, lambertian);
	object_list[object_index] = tri;
	++object_index;

	scene s = {};
	s.objects = object_list;
	s.object_count = object_index;
	return s;
}

scene TestTextures(memory_region *region, thread_context *thread) {
	int object_count = 0;

	material *lambertian = add_lambertian(region);
	material *glass = add_glass(region, 1.52f);
	material *blurrymetal = add_metal(region, 0.69f);

	texture *img = add_image_texture(region, "../resource/text1.jpg", 3);
	texture *img2 = add_image_texture(region, "../resource/earth.jpg", 3);
	object *tri = add_triangle(region, vec3_new(-5.0f, 0.0f, 5.0f), vec3_new(-5.0f, -5.0f, 0.0f), vec3_new(-5.0f, 5.0f, 0.0f), img2, lambertian); 
	++object_count;

	texture *uvtxt = add_uv_texture(region);
	object *sph = add_sphere(region, vec3_new(-8.0f, 0.0f, -2.0f), 1.5f, img2, lambertian);
	++object_count;
	object *sph2 = add_sphere(region, vec3_new(-8.0f, -3.0f, -2.0f), 1.5f, img, blurrymetal);
	++object_count;
	object *sph3 = add_sphere(region, vec3_new(-8.0f, 3.0f, -2.0f), 1.5f, img2, glass);
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

scene TestGlassAndTextures(memory_region *region, thread_context *thread) {
	int object_count = 10;
	int object_index = 0;
	object **object_list = (object **)malloc(sizeof(object *) * object_count);

	material *lambertian = add_lambertian(region);
	material *clearmetal = add_metal(region, 0.0f);
	material *glass = add_glass(region, 1.52f);

	vec3 x_offset = vec3_new(-15.0f, 0.0f, 0.0f);

	texture *white = add_color_texture(region, COLOR_WHITE);
	texture *earth = add_image_texture(region, "../resource/earth.jpg", 3);

	object *glass_sphere = add_sphere(region, vec3_new(-5.0f, 0.0f, 0.0f), 1.2f, white, glass);
	object_list[object_index] = glass_sphere;
	++object_index;

	object *center_sphere = add_sphere(region, vec3_add(x_offset, vec3_new(0.0f, 0.0f, 0.0f)), 1.0f, earth, clearmetal);
	object_list[object_index] = center_sphere;
	++object_index;

	vec3 offset = vec3_new(0.0f, 1.0f, 0.0f);

	int outer_count = 7;
	float outer_radius = 2.1f;
	float r = 0.9f;
	texture *rainbow[7];
	rainbow[0] = add_color_texture(region, COLOR_RED);
	rainbow[1] = add_color_texture(region, fcolor_new(0.7f, 0.5f, 0.0f));
	rainbow[2] = add_color_texture(region, fcolor_new(0.8f, 0.8f, 0.0f));
	rainbow[3] = add_color_texture(region, fcolor_new(0.0f, 1.0f, 0.0f));
	rainbow[4] = add_color_texture(region, fcolor_new(0.0f, 0.0f, 1.0f));
	rainbow[5] = add_color_texture(region, fcolor_new(0.12f, 0.0f, 0.6f));
	rainbow[6] = add_color_texture(region, fcolor_new(0.77f, 0.0f, 0.77f));
	float theta_inc = 360.0f / (float)(outer_count);
	for (int i = 0; i < outer_count; ++i) {
		float theta = -degrees_to_radians(theta_inc * (float)i);
		vec3 center = vec3_mul(vec3_new(0.0f, cosf(theta), sinf(theta)), outer_radius);
		center = vec3_add(x_offset, center);
		object *o = add_sphere(region, center, r, rainbow[i], lambertian);
		object_list[object_index] = o;
		++object_index;
	}

	texture *gray = add_color_texture(region, fcolor_new(0.6f, 0.6f, 0.6f));
	object *tri = add_triangle(region, vec3_new(-20.0f, -20.0f, -5.0f), vec3_new(-20.0f, 0.0f, 20.0f), vec3_new(-20.0f, 20.0f, -5.0f), gray, lambertian);
	object_list[object_index] = tri;
	++object_index;

	scene s = {};
	s.objects = object_list;
	s.object_count = object_index;
	return s;
}

scene MiscTextureTest(memory_region *region, scene *s, vec3 *o, vec3 *t, thread_context *thread) {
	int max_objects = 256;
	int obj_index = 0;
	object **objects = (object **)malloc(sizeof(object *) * max_objects);


	material *lamb = add_lambertian(region);
	material *light = add_diffuse_light(region);
	material *glass = add_glass(region, 1.52f);

	texture *white_light = add_color_texture(region, COLOR_VALUE(7.0f));

	texture *white = add_color_texture(region, COLOR_VALUE(1.0f));
	texture *pink = add_color_texture(region, fcolor_new(0.8f, 0.5f, 0.42f));
	texture *teal = add_color_texture(region, fcolor_new(0.25f, 0.65f, 0.68f));
	texture *green = add_color_texture(region, fcolor_new(0.3f, 1.0f, 0.5f));
	texture *orange = add_color_texture(region, fcolor_new(0.8f, 0.75f, 0.4f));

	texture *texturelist[32]; // size 32 just in case
	noise *shared_perlin = add_perlin_noise(region, 64, thread);
	noise *shared_simplex = add_simplex_noise(region, 64, thread);
	int txt_i = 0;
	texturelist[txt_i++] = add_colored_noise_texture(region, shared_perlin, 1.0f, green);
	texturelist[txt_i++] = add_colored_noise_texture(region, shared_perlin, 2.0f, green);
	texturelist[txt_i++] = add_colored_noise_texture(region, shared_perlin, 4.0f, green);
	texturelist[txt_i++] = add_colored_noise_texture(region, shared_perlin, 8.0f, green);

	texturelist[txt_i++] = add_colored_noise_texture(region, shared_simplex, 1.0f, green);
	texturelist[txt_i++] = add_colored_noise_texture(region, shared_simplex, 2.0f, green);
	texturelist[txt_i++] = add_colored_noise_texture(region, shared_simplex, 4.0f, green);
	texturelist[txt_i++] = add_colored_noise_texture(region, shared_simplex, 8.0f, green);

	texturelist[txt_i++] = add_marbled_noise_texture(region, shared_perlin, 1.0f, orange);
	texturelist[txt_i++] = add_marbled_noise_texture(region, shared_perlin, 2.0f, orange);
	texturelist[txt_i++] = add_marbled_noise_texture(region, shared_perlin, 4.0f, orange);
	texturelist[txt_i++] = add_marbled_noise_texture(region, shared_perlin, 8.0f, orange);

	texturelist[txt_i++] = add_marbled_noise_texture(region, shared_simplex, 1.0f, orange);
	texturelist[txt_i++] = add_marbled_noise_texture(region, shared_simplex, 2.0f, orange);
	texturelist[txt_i++] = add_marbled_noise_texture(region, shared_simplex, 4.0f, orange);
	texturelist[txt_i++] = add_marbled_noise_texture(region, shared_simplex, 8.0f, orange);

	texturelist[txt_i++] = add_noise_sincos_texture(region, shared_perlin, 1.0f, pink, teal);
	texturelist[txt_i++] = add_noise_sincos_texture(region, shared_perlin, 2.0f, pink, teal);
	texturelist[txt_i++] = add_noise_sincos_texture(region, shared_perlin, 4.0f, pink, teal);
	texturelist[txt_i++] = add_noise_sincos_texture(region, shared_perlin, 8.0f, pink, teal);

	texturelist[txt_i++] = add_noise_sincos_texture(region, shared_simplex, 1.0f, pink, teal);
	texturelist[txt_i++] = add_noise_sincos_texture(region, shared_simplex, 2.0f, pink, teal);
	texturelist[txt_i++] = add_noise_sincos_texture(region, shared_simplex, 4.0f, pink, teal);
	texturelist[txt_i++] = add_noise_sincos_texture(region, shared_simplex, 8.0f, pink, teal);


	texturelist[txt_i] = add_fbm_modifier(region, shared_perlin, 0.5f, 7, texturelist[txt_i - 8]);
	++txt_i;
	texturelist[txt_i] = add_fbm_modifier(region, shared_perlin, 0.5f, 9, texturelist[txt_i - 8]);
	++txt_i;
	texturelist[txt_i] = add_fbm_modifier(region, shared_perlin, 0.5f, 12, texturelist[txt_i - 8]);
	++txt_i;
	texturelist[txt_i] = add_fbm_modifier(region, shared_perlin, 0.5f, 24, texturelist[txt_i - 8]);
	++txt_i;

	texturelist[txt_i] = add_fbm_modifier(region, shared_simplex, 0.5f, 7, texturelist[txt_i - 8]);
	++txt_i;
	texturelist[txt_i] = add_fbm_modifier(region, shared_simplex, 0.5f, 9, texturelist[txt_i - 8]);
	++txt_i;
	texturelist[txt_i] = add_fbm_modifier(region, shared_simplex, 0.5f, 12, texturelist[txt_i - 8]);
	++txt_i;
	texturelist[txt_i] = add_fbm_modifier(region, shared_simplex, 0.5f, 24, texturelist[txt_i - 8]);
	++txt_i;




	vec3 points[32];
	float x_offset = 1.25f; 
	float y_offset = 1.25f;
	float z_offset = 1.0f;
	float radius = 1.0f;
	int index = 0;
	int num_cols = 8, num_rows = 4;
	for (int i = 0; i < num_cols; ++i) {
		for (int j = 0; j < num_rows; ++j) {
			points[index++] = vec3_new((float)i * (x_offset + radius), (float)j * (y_offset + radius), z_offset);
		}
	}

	vec3 center = vec3_new((x_offset + radius) * ((float)num_cols-1) * 0.5f,(radius + y_offset) * ((float)num_rows-1) * 0.5f, 0.0f);
	*o = vec3_new(center.x, center.y, 22.0f);
	*t = center;


	for (index = 0; index < txt_i; ++index) {
		objects[index] = add_sphere(region, points[index], radius, texturelist[index], lamb);
	}

	s->objects = objects;
	s->object_count = index;



	return *s;
}


scene FBM_Test(memory_region *region, scene *s, vec3 *o, vec3 *t, thread_context *thread) {
	int max_objects = 32;
	int obj_index = 0;
	object **objects = (object **)malloc(sizeof(object *) * max_objects);

	noise *shared_perlin = add_perlin_noise(region, 256, thread);
	noise *shared_simp = add_simplex_noise(region, 32, thread);

	material *lamb = add_lambertian(region);
	material *light = add_diffuse_light(region);
	material *mirror = add_metal(region, 0.02f);
	material *metal = add_metal(region, 0.88f);
	material *blurry_mirror = add_metal(region, 0.32f);

	texture *white_light = add_color_texture(region, COLOR_VALUE(7.0f));

	texture *white = add_color_texture(region, COLOR_VALUE(1.0f));
	texture *gray = add_color_texture(region, COLOR_VALUE(0.54f));
	texture *pink = add_color_texture(region, fcolor_new(0.8f, 0.5f, 0.42f));
	texture *yellow = add_color_texture(region, fcolor_new(0.8f, 0.8f, 0.32f));
	texture *orange = add_color_texture(region, fcolor_new(0.8f, 0.6f, 0.32f));
	texture *purple = add_color_texture(region, fcolor_new(0.8f, 0.5f, 0.82f));
	texture *teal = add_color_texture(region, fcolor_new(0.25f, 0.65f, 0.68f));
	texture *green = add_color_texture(region, fcolor_new(0.3f, 1.0f, 0.5f));

	texture *tile = add_uv_checker_texture(region, orange, white, 16.0f);
	texture *tile2 = add_uv_checker_texture(region, green, gray, 256.0f);
	texture *earth = add_image_texture(region, "../resource/earth.jpg", 3);
	texture *perlin = add_noise_sincos_texture(region, shared_perlin, 2.0f, pink, yellow);
	texture *perlin2 = add_noise_texture(region, shared_perlin, 1.0f);
	texture *perlin3 = add_noise_sincos_texture(region, shared_perlin, 1.0f, pink, white);
	texture *normal = add_normal_texture(region);
	texture *snormal = add_signed_normal_texture(region);
	texture *xz_levelcurves = add_level_curve_texture(region, vec3_new(0.1f, 0.0f, 0.1f), vec3_new(0.05f, 0.0f, 0.05f), yellow, teal);

	// TODO : make loop to test many different scales quickly

	object *ground = add_triangle(region, vec3_new(-1000.0f, -1000.0f, 0.0f), vec3_new(-1000.0f, 1000.0f, 0.0f), vec3_new(1000.0f, 0.0f, 0.0f), tile2, blurry_mirror);

	float x_offset = 3.0f; 
	float y_offset = 3.0f;
	float radius = 1.0f;
	float z_offset = radius;
	float offset_scale[4] = {0.0f, 0.2f, 0.5f, 1.0f};
	int index = 0;
	vec3 points[24];
	int num_cols = 6, num_rows = 4;
	for (int i = 0; i < num_cols; ++i) {
		for (int j = 0; j < num_rows; ++j) {
			points[index++] = vec3_new((float)i * (x_offset + radius), (float)j * (y_offset + radius) + offset_scale[j], z_offset + offset_scale[j]);
		}
	}
	texture *textures[6] = {snormal, perlin2, xz_levelcurves, perlin, tile, tile};
	material *materials[6] = {lamb, lamb, lamb, lamb, metal, lamb};

	index = 0;

	int octaves = 24;
	float perlin_scale = 0.5f;
	int hurst = 1.0f;
	for (int c = 0; c < num_cols; ++c) {
			object *control_sphere = add_sphere(region, points[index++], radius, textures[c], materials[c]);
			object *source_sphere_1 = add_sphere(region, points[index++], radius, textures[c], materials[c]);
			object *source_sphere_2 = add_sphere(region, points[index++], radius, textures[c], materials[c]);
			object *source_sphere_3 = add_sphere(region, points[index++], radius, textures[c], materials[c]);
			object *fbm_sphere_1 = add_fbm_sphere(region, shared_simp, perlin_scale, offset_scale[1], hurst, octaves, source_sphere_1);
			object *fbm_sphere_2 = add_fbm_sphere(region, shared_simp, perlin_scale, offset_scale[2], hurst, octaves, source_sphere_2);
			object *fbm_sphere_3 = add_fbm_sphere(region, shared_simp, perlin_scale, offset_scale[3], hurst, octaves, source_sphere_3);
			objects[obj_index++] = control_sphere;
			objects[obj_index++] = fbm_sphere_1;
			objects[obj_index++] = fbm_sphere_2;
			objects[obj_index++] = fbm_sphere_3;
	}

	float z = 12.0f;
	vec3 pt_a = vec3_new(-3.0f + x_offset, -3.0f, z);
	vec3 pt_b = vec3_new(3.0f + x_offset, -3.0f, z);
	vec3 pt_c = vec3_new(-3.0f + x_offset, 3.0f, z);
	vec3 pt_d = vec3_new(3.0f + x_offset, 3.0f, z);
	object *light_tri = add_single_sided_triangle(region, pt_b, pt_a, pt_d, white_light, light);
	object *light_tri2 = add_single_sided_triangle(region, pt_a, pt_c, pt_d, white_light, light);


	vec3 floor_a = vec3_new(x_offset - 3.0f, -y_offset, -z_offset);
	vec3 floor_b = vec3_new(x_offset - 3.0f, -0.5f*y_offset, -z_offset);
	vec3 floor_c = vec3_new(x_offset + 3.0f, -0.5f*y_offset, -z_offset);
	vec3 floor_d = vec3_new(x_offset + 3.0f, -y_offset, -z_offset);
	object *floor_left1 = add_triangle(region, floor_a, floor_d, floor_b, green, lamb);
	object *floor_left2 = add_triangle(region, floor_d, floor_b, floor_c, yellow, lamb);
	object *angled_tri_left = add_triangle(region, floor_d, floor_a, vec3_new(x_offset, -(1.5f * y_offset), -0.5f * z_offset), gray, lamb);

	vec3 floor2_a = vec3_new(x_offset - 3.0f, y_offset, -z_offset);
	vec3 floor2_b = vec3_new(x_offset - 3.0f, 0.5f*y_offset, -z_offset);
	vec3 floor2_c = vec3_new(x_offset + 3.0f, 0.5f*y_offset, -z_offset);
	vec3 floor2_d = vec3_new(x_offset + 3.0f, y_offset, -z_offset);
	object *floor_right1_src = add_triangle(region, floor2_a, floor2_d, floor2_b, green, lamb);
	object *floor_right2_src = add_triangle(region, floor2_c, floor2_b, floor2_d, yellow, lamb);
	object *angled_tri_right_src = add_triangle(region, floor2_a, floor2_d, vec3_new(x_offset, 1.5f * y_offset, -0.5f * z_offset), gray, lamb);
	object *floor_right1 = add_fbm_shape(region, shared_simp, 0.1f, 0.1f, 0.5f, 24, floor_right1_src);
	object *floor_right2 = add_fbm_shape(region, shared_simp, 0.1f, 0.2f, 0.5f, 24, floor_right2_src);
	object *angled_tri_right = add_fbm_shape(region, shared_simp, 0.1f, 0.5f, 0.5f, 24, angled_tri_right_src);


	object *light_sphere = add_sphere(region, vec3_new(-5.0, 40.0, 15.0), 8.0, white_light, light);

	//objects[obj_index++] = light_tri;
	//objects[obj_index++] = light_tri2;
	
	//objects[obj_index++] = light_sphere;

	objects[obj_index++] = ground;

	s->objects = objects;
	s->object_count = obj_index;

	vec3 diagonal = vec3_add(points[num_rows - 1], points[num_rows * (num_cols - 1)]);
	vec3 center = vec3_add(points[num_rows - 1], vec3_mul(points[num_rows * (num_cols - 1)], 0.9f));
	//vec3 center = vec3_new((x_offset + radius) * ((float)num_cols-1) * 0.5f,(radius + y_offset) * ((float)num_rows-1) * 0.5f, 0.0f);
	//*o = vec3_new(-(((float)num_rows) * (x_offset + radius)) - 1.0f, ((float)num_cols) * (radius + y_offset) + 12.0f, 15.0f);
	*o = vec3_add(vec3_add(vec3_mul(diagonal, 1.8f), center), vec3_new(0.0, 0.0, 12.0f));
	*t = center;
	return *s;
}

/*
scene FBM_Test2(memory_region *region, scene *s, vec3 *o, vec3 *t, thread_context *thread) {
	int max_objects = 16;
	int obj_index = 0;
	object **objects = (object **)malloc(sizeof(object *) * max_objects);

	perlin *shared_perlin = add_perlin(region, 256);

	material *lamb = add_lambertian(region);
	material *lambertian = lamb;
	material *light = add_diffuse_light(region);
	material *glass = add_glass(region, 1.52f);
	material *mirror = add_metal(region, 0.02f);

	texture *white_light = add_color_texture(region, COLOR_VALUE(7.0f));

	texture *white = add_color_texture(region, COLOR_VALUE(1.0f));
	texture *gray = add_color_texture(region, COLOR_VALUE(0.54f));
	texture *pink = add_color_texture(region, fcolor_new(0.8f, 0.5f, 0.42f));
	texture *yellow = add_color_texture(region, fcolor_new(0.8f, 0.8f, 0.32f));
	texture *orange = add_color_texture(region, fcolor_new(0.8f, 0.6f, 0.32f));
	texture *purple = add_color_texture(region, fcolor_new(0.8f, 0.5f, 0.82f));
	texture *teal = add_color_texture(region, fcolor_new(0.25f, 0.65f, 0.68f));
	texture *green = add_color_texture(region, fcolor_new(0.3f, 1.0f, 0.5f));

	texture *tile = add_uv_checker_texture(region, orange, teal, 16.0f);
	texture *earth = add_image_texture(region, "../resource/earth.jpg", 3);
	texture *perlin = add_perlin_sincos_texture(region, shared_perlin, 16.0f, pink, yellow);

	// TODO : add a triangle with fbm stuff
	// TODO : for fbm_shapes add a scale parameter
	// NOTE : 1.0f is way better than 0.5f for messing with objects
	//
	object *perlin_sphere = add_sphere(region, vec3_new(-10.0f, 0.0f, -101.0f), 100.0f, perlin, lamb);
	object *planet = add_fbm_sphere(region, shared_perlin, 1.0f, 24, 1.5f, perlin_sphere);

	object *tileball= add_sphere(region, vec3_new(-6.0f, 0.0f, 2.0f), 1.0f, tile, lamb);
	object *fbm_sphere4 = add_fbm_sphere(region, shared_perlin, 1.0f, 24, 0.1f, tileball);

	float z = 12.0f;
	vec3 pt_a = vec3_new(-8.0f, -3.0f, z);
	vec3 pt_b = vec3_new(-5.0f, -3.0f, z);
	vec3 pt_c = vec3_new(-8.0f, 3.0f, z);
	vec3 pt_d = vec3_new(-5.0f, 3.0f, z);
	object *light_tri = add_single_sided_triangle(region, pt_b, pt_a, pt_d, white_light, light);
	object *light_tri2 = add_single_sided_triangle(region, pt_a, pt_c, pt_d, white_light, light);


	float trifloat = 3.0f;
	vec3 floor_a = vec3_new(0.0f, -trifloat, 0.0f);
	vec3 floor_b = vec3_new(0.0f, trifloat, 0.0f);
	vec3 floor_c = vec3_new(-trifloat, -trifloat, 0.0f);
	vec3 floor_d = vec3_new(-trifloat, trifloat, 0.0f);
	object *floor = add_triangle(region, floor_a, floor_d, floor_c, tile, lambertian);
	object *floor2 = add_triangle(region, floor_a, floor_b, floor_d, tile, lambertian);
	object *fbm_floor = add_fbm_shape(region, shared_perlin, 0.5f, 24, 0.2f, floor2);

	object *angled_tri = add_triangle(region, floor_a, floor_c, vec3_new(trifloat*0.5f, -(trifloat*2.0f), trifloat), tile, lamb);
	object *angled_tri2 = add_triangle(region, floor_b, floor_d, vec3_new(trifloat*0.5f, trifloat*2.0f, trifloat), tile, lamb);
	object *fbm_angled_tri = add_fbm_shape(region, shared_perlin, 0.5f, 24, 0.1f, angled_tri2);


	//objects[obj_index++] = light_tri;
	//objects[obj_index++] = light_tri2;

	objects[obj_index++] = fbm_sphere4;
	objects[obj_index++] = floor;
	objects[obj_index++] = fbm_floor;
	objects[obj_index++] = planet;
	objects[obj_index++] = angled_tri;
	objects[obj_index++] = fbm_angled_tri;

	s->objects = objects;
	s->object_count = obj_index;

	*o = vec3_new(5.0f, 0.0f, 1.0f);
	*t = vec3_new(-5.0f, 0.0f, 1.0f);
	return *s;
}
*/

scene FBM_Triangle(memory_region *region, scene *s, vec3 *o, vec3 *t, thread_context *thread) {
	int max_objects = 16;
	int obj_index = 0;
	object **objects = (object **)malloc(sizeof(object *) * max_objects);

	perlin *shared_perlin = add_perlin(region, 256, thread);
	noise *shared_noise = add_simplex_noise(region, 256, thread);

	material *lamb = add_lambertian(region);
	material *light = add_diffuse_light(region);

	texture *white_light = add_color_texture(region, COLOR_VALUE(7.0f));
	texture *sunlight = add_color_texture(region, fcolor_new(7.0f, 6.0f, 2.0f));

	texture *white = add_color_texture(region, COLOR_VALUE(1.0f));
	texture *yellow = add_color_texture(region, fcolor_new(0.8f, 0.8f, 0.32f));
	texture *orange = add_color_texture(region, fcolor_new(0.8f, 0.6f, 0.32f));
	texture *brown = add_color_texture(region, fcolor_new(0.4f, 0.33f, 0.05f));
	texture *teal = add_color_texture(region, fcolor_new(0.25f, 0.65f, 0.68f));
	texture *green = add_color_texture(region, fcolor_new(0.3f, 1.0f, 0.5f));
	texture *red = add_color_texture(region, COLOR_RED);
	texture *blue = add_color_texture(region, COLOR_BLUE);

	texture *tile = add_uv_checker_texture(region, orange, teal, 16.0f);
	texture *wavey = add_noise_sincos_texture(region, shared_noise, 1.0f, green, brown);
	texture *perlin = add_noise_texture(region, shared_noise, 1.0f);
	texture *normal = add_normal_texture(region);
	texture *levelcurve = add_level_curve_texture(region, vec3_new(0.0f, 0.8f, 0.0f), vec3_new(0.0f, 0.05f, 0.0f), white, normal);
	texture *levelcurve2 = add_level_curve_texture(region, vec3_new(0.0f, 0.0f, 0.5f), vec3_new(0.0f, 0.0f, 0.04f), levelcurve, white);

	// TODO : add a triangle with fbm stuff
	// TODO : for fbm_shapes add a scale parameter
	// NOTE : 1.0f is way better than 0.5f for messing with objects
	

	object *sun = add_sphere(region, vec3_new(8.0f, 8.0f, 6.5f), 5.0f, sunlight, light);

	//object *landscape_control = add_triangle(region, vec3_new(10.0f, 0.0f, 0.0f), vec3_new(-5.0f, -5.0f, 0), vec3_new(-5.0f, 5.0f, 0.0f), normal, lamb);

	object *landscape_tri = add_single_sided_triangle(region, vec3_new(10.0f, 0.0f, 0.0f), vec3_new(-10.0f, -10.0f, 0), vec3_new(-10.0f, 10.0f, 0.0f), levelcurve, lamb);
	object *landscape = add_fbm_shape(region, shared_noise, 0.5f, 2.0f, 0.5f, 24, landscape_tri);

	object *backdrop_left = add_triangle(region, vec3_new(-10.0f, 20.0f, 20.0f), vec3_new(-10.0f, -20.0f, -20.0f), vec3_new(-10.0f, -5.0f, 20.0f), tile, lamb);
	object *backdrop_right = add_triangle(region, vec3_new(-10.0f, 20.0f, -20.0f), vec3_new(-10.0f, -20.0f, -20.0f), vec3_new(-10.0f, 20.0f, 20.0f), tile, lamb);

	object *reference_normal_sphere = add_sphere(region, vec3_new(-1.0f, -5.0f, -1.0f), 1.0f, normal, lamb);

	//objects[obj_index++] = sun;
	objects[obj_index++] = reference_normal_sphere;
	//objects[obj_index++] = landscape_left;
	//objects[obj_index++] = landscape_control;
	//objects[obj_index++] = lc_sphere;
	objects[obj_index++] = landscape;
	//objects[obj_index++] = landscape_h;
	objects[obj_index++] = backdrop_left;
	objects[obj_index++] = backdrop_right;

	s->objects = objects;
	s->object_count = obj_index;

	*o = vec3_new(12.0f, 0.0f, 2.0f);
	*t = vec3_new(-2.0f, 0.0f, 0.0f);
	return *s;
}

scene FBM_NormalTest(memory_region *region, scene *s, vec3 *o, vec3 *t, thread_context *thread) {
	int max_objects = 16;
	int obj_index = 0;
	object **objects = (object **)malloc(sizeof(object *) * max_objects);

	perlin *shared_perlin = add_perlin(region, 256, thread);
	noise *shared_noise = add_simplex_noise(region, 256, thread);

	material *lamb = add_lambertian(region);
	material *mirror = add_metal(region, 0.01f);

	texture *white = add_color_texture(region, COLOR_VALUE(0.7f));
	texture *gold = add_color_texture(region, fcolor_new(0.8f, 0.78f, 0.3f));
	texture *bronze = add_color_texture(region, fcolor_new(0.9f, 0.7f, 0.5f));
	texture *grn = add_color_texture(region, COLOR_GREEN);
	texture *red = add_color_texture(region, COLOR_RED);
	texture *blue = add_color_texture(region, COLOR_BLUE);
	texture *normal = add_normal_texture(region);
	texture *snormal = add_signed_normal_texture(region);
	texture *earth = add_image_texture(region, "../resource/earth.jpg", 3);
	texture *tile = add_uv_checker_texture(region, red, grn, 64.0f);
	texture *tile2 = add_uv_checker_texture(region, normal, white, 16.0f);
	texture *dist = add_distance_texture(region, vec3_new(0.0f, 0.0f, 0.0f), 2.0f, NULL, NULL);
	//texture *levelcurve = add_level_curve_texture(region, vec3_new(0.0f, 0.0f, 0.05f), vec3_new(0.0f, 0.0f, 0.01f), white, normal);

	object *sph;
	object *sph2;
	object *sph3;
	if(0) {
	texture *normtxt = normal;
	sph  = add_sphere(region, vec3_new(0.0f, 0.0f, 0.0f), 1.0f, normtxt, lamb);
	sph2 = add_sphere(region, vec3_new(-2.0f, -3.0f, -2.0f), 3.0f, normtxt, lamb);
	sph3 = add_sphere(region, vec3_new(0, 3.0f, -1.5f), 0.5f, normtxt, lamb);
	} else {
	sph  = add_sphere(region, vec3_new(0.0f, 0.0f, 0.0f), 1.0f, gold, mirror);
	sph2 = add_sphere(region, vec3_new(-2.0f, -3.0f, -2.0f), 3.0f, white, mirror);
	sph3 = add_sphere(region, vec3_new(0, 3.0f, -1.5f), 0.5f, white, mirror);
	}
	object *fbm_sph = add_fbm_sphere(region, shared_noise,  2.0f, 0.5f, 1.0f, 5, sph);
	object *fbm_sph2 = add_fbm_sphere(region, shared_noise,  0.2f, 0.2f, 1.0f, 5, sph2);
	object *fbm_sph3 = add_fbm_sphere(region, shared_noise,  0.2f, 0.2f, 0.5f, 5, sph3);

	object *bg = add_sphere(region, vec3_new(110.0f, 0.0f, 0.0f), 100.0f, tile, lamb);
	object *floor = add_triangle(region, vec3_new(10.0f, 0.0f, -1.5f), vec3_new(-10.0f, -10.0f, -1.5f), vec3_new(-10.0f, 10.0f, -1.5f), tile2, lamb);

	//objects[obj_index++] = sph;
	objects[obj_index++] = fbm_sph;
	objects[obj_index++] = fbm_sph2;
	objects[obj_index++] = fbm_sph3;
	objects[obj_index++] = bg;
	//objects[obj_index++] = floor;

	s->objects = objects;
	s->object_count = obj_index;

	vec3 top = vec3_new(1.0f, 0.0f, 5.0f);
	vec3 front = vec3_new(8.0f, 0.0f, 0.0f);
	vec3 side = vec3_new(0.0f, 10.0f, 0.0f);
	*o = front;
	*t = vec3_new(0.0f, 0.0f, 0.0f);
	return *s;
}

scene FBM_Reflection(memory_region *region, scene *s, vec3 *o, vec3 *t, thread_context *thread) {
	int max_objects = 16;
	int obj_index = 0;
	object **objects = (object **)malloc(sizeof(object *) * max_objects);

	perlin *shared_perlin = add_perlin(region, 256, thread);
	noise *shared_noise = add_simplex_noise(region, 256, thread);

	material *lamb = add_lambertian(region);
	material *light = add_diffuse_light(region);
	material *mirror = add_metal(region, 0.01f);

	texture *white_light = add_color_texture(region, COLOR_VALUE(7.0f));

	texture *white = add_color_texture(region, COLOR_VALUE(1.0f));
	texture *normal = add_normal_texture(region);
	texture *simplex_text = add_colored_noise_texture(region, shared_noise, 1.0f, normal);


	object *light_tri = add_triangle(region, vec3_new(-1.0f, 3.0f, 4.0f), vec3_new(-1.0f, -3.0f, 4.0f), vec3_new(1.0f, 0.0f, 4.0f), white_light, light);

	object *sph = add_sphere(region, vec3_new(0.0f, -2.3f, 1.3f), 1.0f, normal, lamb);
	object *fbm_sph = add_fbm_sphere(region, shared_noise,  0.2f, 0.3f, 0.5f, 24, sph);

	object *control_sphere = add_sphere(region, vec3_new(0.0f, 0.0f, 1.1f), 1.0f, normal, lamb);

	object *sph2 = add_sphere(region, vec3_new(0.0f, 2.3f, 1.3f), 1.0f, normal, lamb);
	object *fbm_sph2 = add_fbm_sphere(region, shared_noise,  0.2f, 0.3f, 0.5f, 24, sph2);

	object *floor = add_triangle(region, vec3_new(100.0f, 0.0f, 0.0f), vec3_new(-100.0f, -100.0f, 0.0f), vec3_new(-100.0f, 100.0f, 0.0f), simplex_text, lamb);

	objects[obj_index++] = light_tri;
	objects[obj_index++] = control_sphere;
	objects[obj_index++] = fbm_sph;
	objects[obj_index++] = fbm_sph2;
	//objects[obj_index++] = floor;

	s->objects = objects;
	s->object_count = obj_index;

	*o = vec3_new(5.0f, 0.0f, 1.0f);
	*t = vec3_new(0.0f, 0.0f, 0.0f);
	return *s;
}


scene SimplexNoiseTest(memory_region *region, scene *s, vec3 *o, vec3 *t, thread_context *thread) {
	int max_objects = 16;
	int obj_index = 0;
	object **objects = (object **)malloc(sizeof(object *) * max_objects);

	noise *shared_simplex = add_simplex_noise(region, 256, thread);

	material *lamb = add_lambertian(region);
	material *metal = add_metal(region, 0.44f);

	texture *white = add_color_texture(region, COLOR_VALUE(1.0f));
	texture *blue = add_color_texture(region, COLOR_BLUE);
	texture *red = add_color_texture(region, COLOR_RED);

	texture *simp_1 = add_colored_noise_texture_sized(region, 0.5f, 256, NULL);
	texture *simp_2 = add_colored_noise_texture(region, shared_simplex, 1.0f, white);
	texture *simp_3 = add_colored_noise_texture(region, shared_simplex, 2.0f, NULL);
	texture *simp_4 = add_colored_noise_texture(region, shared_simplex, 16.0f, NULL);

	texture *col_simp_1 = add_colored_noise_texture_sized(region, 0.5f, 256, red);
	texture *col_simp_2 = add_colored_noise_texture(region, shared_simplex, 1.0f, blue);
	texture *col_simp_3 = add_colored_noise_texture(region, shared_simplex, 2.0f, blue);
	texture *col_simp_4 = add_colored_noise_texture(region, shared_simplex, 16.0f, blue);

	float r = 1.0f;
	float m = 1.0f;
	object *a = add_sphere(region, vec3_new(0.0f, -1.5f * (r + m), 1.0f), r, simp_1, lamb);
	object *b = add_sphere(region, vec3_new(0.0f, -0.5f * (r + m), 1.0f), r, simp_2, lamb);
	object *c = add_sphere(region, vec3_new(0.0f, 0.5f * (r + m), 1.0f), r, simp_3, lamb);
	object *d = add_sphere(region, vec3_new(0.0f, 1.5f * (r + m), 1.0f), r, simp_4, lamb);

	object *a2 = add_sphere(region, vec3_new(0.0f, -1.5f * (r + m), -1.0f), r, col_simp_1, lamb);
	object *b2 = add_sphere(region, vec3_new(0.0f, -0.5f * (r + m), -1.0f), r, col_simp_2, lamb);
	object *c2 = add_sphere(region, vec3_new(0.0f, 0.5f * (r + m), -1.0f), r, col_simp_3, lamb);
	object *d2 = add_sphere(region, vec3_new(0.0f, 1.5f * (r + m), -1.0f), r, col_simp_4, lamb);

	objects[obj_index++] = a;
	objects[obj_index++] = b;
	objects[obj_index++] = c;
	objects[obj_index++] = d;
	objects[obj_index++] = a2;
	objects[obj_index++] = b2;
	objects[obj_index++] = c2;
	objects[obj_index++] = d2;

	s->objects = objects;
	s->object_count = obj_index;

	*o = vec3_new(5.0f, 0.0f, 0.0f);
	*t = vec3_new(0.0f, 0.0f, 0.0f);
	return *s;
}

scene Planet(memory_region *region, scene *s, vec3 *o, vec3 *t, thread_context *thread) {
	int max_objects = 16;
	int obj_index = 0;
	object **objects = (object **)malloc(sizeof(object *) * max_objects);

	noise *shared_perlin = add_perlin_noise(region, 32, thread);
	noise *shared_noise = add_simplex_noise(region, 256, thread);

	material *lamb = add_lambertian(region);
	material *mirror = add_metal(region, 0.01f);
	material *light = add_diffuse_light(region);
	
	texture *white_light = add_color_texture(region, COLOR_VALUE(7.0f));

	texture *white = add_color_texture(region, COLOR_VALUE(1.0f));
	texture *pine = add_color_texture(region, fcolor_new(0.2f, 0.55f, 0.24f));
	texture *blue = add_color_texture(region, fcolor_new(0.33f, 0.44f, 0.75f));
	texture *darkblue = add_color_texture(region, fcolor_new(0.02f, 0.04f, 0.45f));
	texture *land_src = add_noise_sincos_texture(region, shared_noise, 8.0f, blue, pine);
	texture *land = add_fbm_modifier(region, shared_noise, 0.5f, 4, land_src);
	texture *normal = add_normal_texture(region);
	texture *snormal = add_signed_normal_texture(region);
	
	vec3 center = vec3_new(0.0f, 0.0f, -100.0f);
	texture *snowcaps = add_distance_texture(region, center, 200.0f, white, land);

	object *sph;
	sph = add_sphere(region, center, 100.0f, snormal, lamb);
	object *fbm_sph = add_fbm_sphere(region, shared_noise,  1.0f/16.0f, 10.0f, 1.0f, 5, sph);

	object *lightsrc = add_triangle(region, vec3_new(-50.0f, 0.0f, 25.0f), vec3_new(50.0f, -50.0f, 25.0f), vec3_new(50.0f, 50.0f, 25.0f), white_light, light);

	//objects[obj_index++] = sph;
	objects[obj_index++] = fbm_sph;
	//objects[obj_index++] = lightsrc;

	s->objects = objects;
	s->object_count = obj_index;
	
	vec3 front = vec3_new(200.0f, 0.0f, -50.0f);
	vec3 side = vec3_new(0.0f, 200.0f, -50.0f);
	vec3 top_front = vec3_new(250.0f, 0.0f, -10.0f);
	vec3 bottom = vec3_new(0.0f, 0.0f, -300.0f);
	*o = front;
	*t = vec3_new(0.0f, 0.0f, -50.0f);
	return *s;
}

scene Test_Quad(memory_region *region, scene *s, vec3 *o, vec3 *t, thread_context *thread) {
	int max_objects = 16;
	int obj_index = 0;
	object **objects = (object **)malloc(sizeof(object *) * max_objects);

	noise *shared_perlin = add_perlin_noise(region, 32, thread);
	noise *shared_noise = add_simplex_noise(region, 256, thread);

	material *lamb = add_lambertian(region);
	material *mirror = add_metal(region, 0.01f);
	material *light = add_diffuse_light(region);
	
	texture *white_light = add_color_texture(region, COLOR_VALUE(7.0f));

	texture *white = add_color_texture(region, COLOR_VALUE(1.0f));
	texture *pine = add_color_texture(region, fcolor_new(0.2f, 0.55f, 0.24f));
	texture *blue = add_color_texture(region, fcolor_new(0.33f, 0.44f, 0.75f));
	texture *darkblue = add_color_texture(region, fcolor_new(0.02f, 0.04f, 0.45f));
	texture *land_src = add_noise_sincos_texture(region, shared_noise, 8.0f, blue, pine);
	texture *land = add_fbm_modifier(region, shared_noise, 0.5f, 4, land_src);
	texture *normal = add_normal_texture(region);
	texture *snormal = add_signed_normal_texture(region);
	texture *tile = add_uv_checker_texture(region, pine, blue, 8.0f);
	texture *uv = add_uv_texture(region);
	point3 a, b, c, d, e, f, g, h;
	a = vec3_new(-1.0, -1.0, -1.0);
	b = vec3_new(-1.0, -1.0, 1.0);
	c = vec3_new(-1.0, 1.0, -1.0);
	d = vec3_new(-1.0, 1.0, 1.0);

	e = vec3_new(2.0, -1.0, -2.0);
	f = vec3_new(2.0, 1.0, -2.0);
	g = vec3_new(-2.0, -1.0, 2.0);
	h = vec3_new(-2.0, 1.0, 2.0);


	object *quad = add_quad(region, a, b, c, d, land, lamb);
	object *quad2 = add_quad(region, b, g, d, h, normal, lamb);
	object *quad3 = add_quad(region, a, c, e, f, tile, lamb);
	

	objects[obj_index++] = quad;
	objects[obj_index++] = quad2;
	objects[obj_index++] = quad3;
	s->objects = objects;
	s->object_count = obj_index;
	
	*o = vec3_new(5.0f, 0.0f, 0.0f);
	*t = vec3_new(-1.0f, 0.0f, 0.0f);
	return *s;
}


void CornellBox(memory_region *region, scene *s, vec3 *o, vec3 *t, vec3 *vup, thread_context *tc) {
	int max_objects = 64;
	int obj_index = 0;
	object **objects = (object **)malloc(sizeof(object *) * max_objects);


	material *lamb = add_lambertian(region);
	material *metal = add_metal(region, 0.1);
	material *glass = add_glass(region, 1.52);
	material *light = add_diffuse_light(region);

	texture *white_light = add_color_texture(region, fcolor_new(6.2, 6.5, 6.5));

	texture *red = add_color_texture(region, fcolor_new(0.9, 0.22, 0.2));
	texture *green = add_color_texture(region, fcolor_new(0.3, 0.88, 0.5));
	texture *white = add_color_texture(region, COLOR_VALUE(1.0));
	texture *gray = add_color_texture(region, COLOR_VALUE(0.5));
	texture *tile = add_uv_checker_texture(region, green, red, 16.0);

	// Vertices of the box itself
	float bottom = 0.0, top = 550.0;
	float left = 0.0, right = 555.0;
	float back = 0.0, front = 560.0;

	*o = vec3_new(2.0f * front, right * 0.5, top * 0.5);
	*t = vec3_sub(*o, vec3_new(1.5 * front, 0.0, 0.0));

	vec3 box_right_bottom_front = vec3_new(front, right, bottom);
	vec3 box_left_bottom_front = vec3_new(front, left, bottom);
	vec3 box_right_bottom_back = vec3_new(back, right, bottom);
	vec3 box_left_bottom_back = vec3_new(back, left, bottom);
	vec3 box_right_top_front = vec3_new(front, right, top);
	vec3 box_left_top_front = vec3_new(front, left, top);
	vec3 box_right_top_back = vec3_new(back, right, top);
	vec3 box_left_top_back = vec3_new(back, left, top);

	/* This was the data from cornell.edu. Their coordinates are oriented differently.
	vec3 box_right_bottom_back = vec3_new(552.8, 0.0, 0.0);
	vec3 box_left_bottom_back = vec3_new(0.0, 0.0, 0.0);
	vec3 box_left_bottom_front = vec3_new(0.0, 0.0, 559.2);
	vec3 box_right_bottom_front = vec3_new(549.6, 0.0, 559.2);
	vec3 box_right_top_back = vec3_new(556.0, 548.8, 0.0);
	vec3 box_right_top_front = vec3_new(556.0, 548.8, 559.2);
	vec3 box_left_top_front = vec3_new(0.0, 548.8, 559.2);
	vec3 box_left_top_back = vec3_new(0.0, 548.8, 0.0);
	*/


	vec3 floor_a = box_right_bottom_back;
	vec3 floor_b = box_left_bottom_back;
	vec3 floor_c = box_left_bottom_front;
	vec3 floor_d = box_right_bottom_front;
	object *floor = add_quad(region, floor_a, floor_b, floor_c, floor_d, white, lamb);
	
	float light_radius = right * 0.2;
	vec3 light_a = vec3_new(front * 0.5f - light_radius, right * 0.5f - light_radius, top - 0.005f);
	vec3 light_b = vec3_new(front * 0.5f + light_radius, right * 0.5f - light_radius, top - 0.005f);
	vec3 light_c = vec3_new(front * 0.5f - light_radius, right * 0.5f + light_radius, top - 0.005f);
	vec3 light_d = vec3_new(front * 0.5f + light_radius, right * 0.5f + light_radius, top - 0.005f);
	object *light_on_ceiling = add_ss_quad(region, light_b, light_a, light_c, light_d, white_light, light);

	vec3 ceiling_a = box_right_top_back;
	vec3 ceiling_b = box_right_top_front;
	vec3 ceiling_c = box_left_top_front;
	vec3 ceiling_d = box_left_top_back;
	object *ceiling = add_quad(region, ceiling_a, ceiling_b, ceiling_c, ceiling_d, white, lamb);

	vec3 backwall_a = box_right_bottom_back;
	vec3 backwall_b = box_left_bottom_back;
	vec3 backwall_c = box_right_top_back;
	vec3 backwall_d = box_left_top_back;
	object *backwall = add_quad(region, backwall_a, backwall_b, backwall_c, backwall_d, white, lamb);

	vec3 rightwall_a = box_right_top_back;
	vec3 rightwall_b = box_right_top_front;
	vec3 rightwall_c = box_right_bottom_front;
	vec3 rightwall_d = box_right_bottom_back;
	object *rightwall = add_quad(region, rightwall_a, rightwall_b, rightwall_c, rightwall_d, green, lamb);

	vec3 leftwall_a = box_left_top_front;
	vec3 leftwall_b = box_left_top_back;
	vec3 leftwall_c = box_left_bottom_front;
	vec3 leftwall_d = box_left_bottom_back;
	object *leftwall = add_quad(region, leftwall_a, leftwall_b, leftwall_c, leftwall_d, red, lamb);

	float small_radius = right * 0.15, big_radius = right * 0.18, glass_radius = right * 0.155;
	object *smallsphere = add_sphere(region, vec3_new(front * 0.27, right * 0.27, small_radius + 0.001), small_radius, gray, metal);

	object *bigsphere = add_sphere(region ,vec3_new(front * 0.26, right * 0.75, big_radius + 0.001), big_radius, gray, lamb);

	object *glasssphere = add_sphere(region ,vec3_new(front * 0.7, right * 0.7, glass_radius * 1.0 + 0.001), glass_radius, white, glass);


	objects[obj_index++] = floor;
	objects[obj_index++] = light_on_ceiling;
	objects[obj_index++] = ceiling;
	objects[obj_index++] = backwall;
	objects[obj_index++] = leftwall;
	objects[obj_index++] = rightwall;
	objects[obj_index++] = smallsphere;
	//objects[obj_index++] = bigsphere;
	objects[obj_index++] = glasssphere;

	s->objects = objects;
	s->object_count = obj_index;
}

void SceneSelect(memory_region *region, int selection, scene *s, vec3 *o, vec3 *t, vec3 *vup, thread_context *tc) {
	switch(selection) {
		case 1:
			*s = BlackWhite(region);
			return;
		case 2:
			RainbowCircle(region, s, o, t, tc);
			return;
		case 3:
			*s =  TestReflection(region, tc);
			return;
		case 4:
			*s =  TestGlass(region, tc);
			return;
		case 5:
			*s =  TestTextures(region, tc);
			return;
		case 6:
			*s =  TestGlassAndTextures(region, tc);
			return;
		case 7:
			//*s =  BlackWhiteGlass(region, tc);
			return;
		case 8:
			*s =  SimpleGlass(region, tc);
			return;
		case 9:
			MiscTextureTest(region, s, o, t, tc);
			return;
		case 10:
			//TestLight(region, s, o, t, tc);
			return;
		case 11:
			//TestTriangleLight(region, s, o, t, tc);
			return;
		case 12:
			ScuffedCornellBox(region, s, o, t, tc);
			return;
		case 13:
			FBM_Test(region, s, o, t, tc);
			return;
		case 14:
			//FBM_Test2(region, s, o, t);
		case 15:
			FBM_Triangle(region, s, o, t, tc);
			return;
		case 16:
			FBM_NormalTest(region, s, o, t, tc);
			return;
		case 17:
			SimplexNoiseTest(region, s, o, t, tc);
			return;
		case 18:
			FBM_Reflection(region, s, o, t, tc);
			return;
		case 19:
			Planet(region, s, o, t, tc);
			return;
		case 20:
			Test_Quad(region, s, o, t, tc);
			return;
		default:
			CornellBox(region, s, o, t, vup, tc);
			return;
	}
}

void FreeScene(scene *scene) {
	free(scene->objects);
}
#endif
