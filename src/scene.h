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
		float obj_color = (float)i / (float)object_count;
		object *o = (object *)malloc(sizeof(object));
		//*o = make_sphere(vec3_new((float)i * -2.0 - 2.0, random_float_between(-(float)i, (float)i), random_float_between(-10.0, 10.0)),
		*o = make_sphere(vec3_new((float)i * -2.0 - 2.0, 0.0, (float)i - 5.0),
											0.5,
											fcolor_new(obj_color, obj_color, obj_color));
		object_list[i] = o;
	}
	scene scene = {};
	scene.objects = object_list;
	scene.object_count = object_count;
	return scene;
}

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

void FreeScene(scene *scene) {
	int i = 0;
	while(i < scene->object_count) {
		free(scene->objects[i++]);
	}
	free(scene->objects);
}

#endif
