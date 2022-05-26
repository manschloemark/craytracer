#ifndef OBJECT_LIST_H
#define OBJECT_LIST_H

#include "common.h"
#include "objects.h"
#include "memory.h"

typedef struct {
	object **objects;
	int count, max;
} object_list;

object_list object_list_new(memory_region *region, int max_objects);

int object_list_append(object_list *ol, object *new_object);

int object_list_intersect(object_list *ol, ray *r, hit_record *hitrec, thread_context *thread);

object_list *add_object_list(memory_region *region, int max_objects);

#endif
