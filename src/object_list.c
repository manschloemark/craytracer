#include "object_list.h"

object_list object_list_new(memory_region *region, int max_objects) {
	object_list ol = {};
	ol.objects = (object **)calloc(max_objects, sizeof(object));
	ol.count = 0;
	ol.max = max_objects;
	if (ol.objects == NULL) {
		ol.max = -1;
	}
	return ol;
}

void object_list_free(object_list *ol) {
	if(ol->objects != NULL)
		free(ol->objects);
}

int object_list_add(object_list *ol, object *new_object) {
	if (new_object != NULL) {
		if (ol->count < ol->max) {
			ol->objects[ol->count++] = new_object;
			return 1;
		}
	}
	return 0;
}


int object_list_intersect(object_list *ol, ray *r, hit_record *hitrec, thread_context *thread) {
	int hit = 0;
	int c = 0;
	while(c < ol->count) {
		hit |= ol->objects[c]->Intersect(ol->objects[c], r, hitrec, thread);
		++c;
	}
	return hit;
}
