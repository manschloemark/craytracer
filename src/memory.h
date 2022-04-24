/* memory.h : this will allow me to create objects, meterials, and textures
 without worring about the details too much. */

#ifndef MEMORY_REGION_H
#define MEMORY_REGION_H
#include <stdlib.h>
#include <string.h> // for memcpy

// Convenient sizes
#define KILOBYTES(bytes) 1024*bytes
#define MEGABYTES(bytes) 1024*KILOBYTES(bytes)
#define GIGABYTES(bytes) 1024*MEGABYTES(bytes)

typedef struct {
	void *mem;
	int max_size, used;
} memory_region;

memory_region make_memory_region(int start_size);

void *memory_region_add(memory_region *region, void *item, int item_size);

void FreeMemoryRegion(memory_region *region);

#endif
