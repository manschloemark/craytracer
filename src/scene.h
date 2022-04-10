#ifndef SCENE_H
#define SCENE_H

#include "common.h"
#include "color.h"
#include "objects.h"

typedef struct {
	object **objects;
	int object_count;
} scene;

scene TestScene() {
	int object_count = 10;
	object **object_list = (object **)malloc(sizeof(object *) * object_count);

	int i;
	for(i = 0; i < object_count; ++i) {
		object *o = (object *)malloc(sizeof(object));
		*o = make_sphere(vec3_new(-random_float_between(2.0, 20.0), random_float_between(-(float)i, (float)i), random_float_between(-10.0, 10.0)), random_float() * 2.0, fcolor_new(random_float(), random_float(), random_float()));
		object_list[i] = o;
	}
	scene scene = {};
	scene.objects = object_list;
	scene.object_count = object_count;
	return scene;
}

void FreeScene(scene *scene) {
	int i = 0;
	while(i < scene->object_count) {
		free(scene->objects[i++]);
	}
	free(scene->objects);
}

#endif
