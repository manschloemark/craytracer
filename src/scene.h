#ifndef SCENE_H
#define SCENE_H

#include "common.h"
#include "color.h"
#include "objects.h"
#include "material.h"

#include "memory.h"

typedef struct {
	object **objects;
	int object_count;
} scene;

scene RandomTestScene(memory_region *region) {
	int object_count = 10;
	object **object_list = (object **)malloc(sizeof(object *) * object_count);

	material diff = make_diffuse();
	material *matptr = (material *)memory_region_add(region, &diff, sizeof(material));

	int i;
	for(i = 0; i < object_count / 2; ++i) {
		object o = make_sphere(vec3_new((float)i * -2.0 - 2.0,
											random_float_between(-(float)i, (float)i),
											random_float_between(-10.0, 10.0)),
										random_float() + 0.2,
										fcolor_new(random_float(), random_float(), random_float()), matptr);
		object_list[i] = (object *)memory_region_add(region, &o, sizeof(object));
	}

	for(; i < object_count; ++i) {
		float anchor = random_float_between(-5.0, 5.0);
		object o = make_triangle(
				vec3_new(random_float_between(-8, -5), anchor + random_float_between(0, 1), anchor + random_float_between(0, 4)),
				vec3_new(random_float_between(-8, -3), anchor - random_float_between(-2, 2), anchor + random_float_between(-1, 2)),
				vec3_new(random_float_between(-8, -3), anchor + random_float_between(0, 4), anchor + random_float_between(0, 4)),
											fcolor_new(random_float(), random_float(), random_float()), matptr);
		object_list[i] = (object *)memory_region_add(region, &o, sizeof(object));
	}
	
	scene scene = {};
	scene.objects = object_list;
	scene.object_count = object_count;
	return scene;
}

scene BlackWhite(memory_region *region) {
	int obj_ct = 0;
	material diff = make_diffuse();
	material *matptr = (material *)memory_region_add(region, &diff, sizeof(material));

	object white_sphere = make_sphere(vec3_new(-5.0, -1.0, 0.5), 1.0, fcolor_new(1.0, 1.0, 1.0), matptr);
	object *white_sphereptr = (object *)memory_region_add(region, &white_sphere, sizeof(object));
	++obj_ct;

	object black_sphere = make_sphere(vec3_new(-5.0, 1.0, 0.5), 1.0, fcolor_new(0.0, 0.0, 0.0), matptr);
	object *black_sphereptr = (object *)memory_region_add(region, &black_sphere, sizeof(object));
	++obj_ct;

	object base = make_triangle(vec3_new(20.0, 0.0, -5.0), vec3_new(-20.0, -20.0, -0.5), vec3_new(-20.0, 20.0, -0.5), fcolor_new(0.3, 0.5, 0.33), matptr);
	object *baseptr = (object *)memory_region_add(region, &base, sizeof(object));
	++obj_ct;

	object **objects = (object **)malloc(obj_ct * sizeof(object));

	objects[0] = white_sphereptr;
	objects[1] = black_sphereptr;
	objects[2] = baseptr;

	scene s = {};

	s.objects = objects;
	s.object_count = obj_ct;
	return s;
}

/*
scene TestScene2() {
	object **object_list = (object **)malloc(sizeof(object *));
	object *o = (object *)malloc(sizeof(object));
	*o = make_sphere(vec3_new(-2.0, 0.0, 0.0), 1.0, fcolor_new(1.0, 0.0, 0.0));
	object_list[0] = o;
	scene scene = {};
	scene.objects = object_list;
	scene.object_count = 1;
	return scene;
}

scene TestScene3() {
	object **object_list = (object **)malloc(sizeof(object *) * 2);
	object *o = (object *)malloc(sizeof(object));
	*o = make_sphere(vec3_new(-2.0, -1.5, 0.0), 1.0, fcolor_new(1.0, 0.0, 0.0));
	object_list[0] = o;
	object *o2 = (object *)malloc(sizeof(object));
	*o2 = make_sphere(vec3_new(-2.0, 1.5, 0.0), 1.0, fcolor_new(1.0, 0.0, 0.0));
	object_list[1] = o2;
	scene scene = {};
	scene.objects = object_list;
	scene.object_count = 2;
	return scene;
}

scene TestTriangle() {
	object **object_list = (object **)malloc(sizeof(object *));
	object *o = (object *)malloc(sizeof(object));
	*o = make_triangle(vec3_new(-2.0, 0.0, -1.0), vec3_new(-2.0, 0.0, 1.0), vec3_new(-2.0, 2.0, 0.0), fcolor_new(1.0, 0.0, 0.0));
	object_list[0] = o;
	scene scene = {};
	scene.objects = object_list;
	scene.object_count = 1;
	return scene;

}

scene DebugScene(memory_region *region) {
	int object_count = 3;
	object **object_list = (object **)malloc(object_count * sizeof(object *));
	object o = make_triangle(vec3_new(-2.0, 5.0, 0.0), vec3_new(-2.0, 5.0, 10.0), vec3_new(-2.0, 0.0, -10.0), fcolor_new(1.0, 0.0, 0.0));
	object_list[0] = (object *)memory_region_add(region, &o, sizeof(object));
	object o2 = make_sphere(vec3_new(-4.0, 0.0, 0.0), 1.0, fcolor_new(1.0, 0.0, 0.0));
	object_list[1] = (object *)memory_region_add(region, &o2, sizeof(object));
	object o3 = make_sphere(vec3_new(-10.0, 0.0, 3.0), 4.0, fcolor_new(1.0, 0.0, 0.0));
	object_list[2] = (object *)memory_region_add(region, &o3, sizeof(object));
	scene scene = {};
	scene.objects = object_list;
	scene.object_count = object_count;
	return scene;
}
*/
scene TestMaterial(memory_region *region) {
	int object_count = 2;
	object **object_list = (object **)malloc(object_count * sizeof(object));
	material mat = make_diffuse();
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

void FreeScene(scene *scene) {
	free(scene->objects);
}

#endif
