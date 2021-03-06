#include "memory.h"
/* memory.h : this will allow me to create objects, meterials, and textures
 without worring about the details too much. */
memory_region make_memory_region(int start_size) {
	memory_region region = {};
	region.mem = malloc(start_size);
	region.max_size = start_size;
	region.used = 0;
	return region;
}

void *memory_region_add(memory_region *region, void *item, int item_size) {
	// TODO : fix this! It literally just doesn't work.
	if (region->used + item_size >= region->max_size) {
		return NULL;
		void *new_memory = malloc(region->max_size * 2);
		memcpy(new_memory, region->mem, region->used);
		free(region->mem);
		region->mem = new_memory;
		region->max_size *= 2;
	}
	memcpy((char *)region->mem + region->used, item, item_size);
	void *pointer_to_new_item = (char *)region->mem + region->used;
	region->used += item_size;
	return pointer_to_new_item;
}

void FreeMemoryRegion(memory_region *region) {
	free(region->mem);
	region->used = 0;
	region->max_size = 0;
}
