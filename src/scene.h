#ifndef SCENE_H
#define SCENE_H

#include "vector3.h"
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
	for(i = 0; i < 10; ++i) {
		object *o = (object *)malloc(sizeof(object));
		*o = make_sphere(vec3_new(-5.0 - i, i % 3, i), 0.8, fcolor_new(0.8, (float)i/10.0, (float)i/10.0));
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
