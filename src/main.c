/* main.c ray tracer
 * A very simple minimalistic raytracer that I will word on incrementally.
 * Mark Schloeman
 * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>

#include <math.h>

#include "argparse.h"
#include "common.h"
#include "color.h"
#include "ray.h"

#include "objects.h"
#include "scene.h"

#include "memory.h"

/* Data Types */

static inline float Clamp(float n, float min, float max) {
	return (n < min) ? min : (n > max) ? max : n;
}

static inline int Min(int a, int b) {
	if (a > b) return b;
	return a;
}

static inline int Max(int a, int b) {
	if (a > b) return a;
	return b;
}

/* File I/O */

// TODO :: at some point it would be nice to make it so instead of overwriting
//         this function does it so if filename = test.ppm and test.ppm exists it changes the
//         filename to test(1).ppm or something like that.
void GetUniqueFilename(char *filename) {
	return;
}

FILE *MakePPMFile(char *filename, int height, int width) {
	FILE *newfile = fopen(filename, "w");
	if (!newfile) return NULL;

	char *header;
	header = malloc(sizeof(char) * 64);
	snprintf(header, 64, "P6 %i %i %i\n", width, height, 255);
	fputs(header, newfile);
	fflush(newfile);
	return newfile;
}


void WriteToPPM(fcolor *pixels, int samples, int height, int width, int bytes_per_channel, FILE *dest) {
	unsigned char *temp = malloc(height * width * bytes_per_channel * 3);
	int count;
	for (count = 0; count < height * width * 3; count = count + 3) {
		float r = pixels->r / samples;
		float g = pixels->g / samples;
		float b = pixels->b / samples;
		temp[count] = (unsigned char)(Clamp(r, 0.0, 0.999) * 256);
		temp[count+1] = (unsigned char)(Clamp(g, 0.0, 0.999) * 256);
		temp[count+2] = (unsigned char)(Clamp(b, 0.0, 0.999) * 256);
		++pixels;
	}
	fwrite(temp, height * width * 3, bytes_per_channel, dest);
	fflush(dest);
	free(temp);
}

/* Rendering */
fcolor TraceRay(ray *r, scene *scene) {
	struct hit_record hitrec = {};
	hitrec.t_min = 0.0;
	hitrec.t = 123123902.0; // TODO : make some const to use for this instead
	int c = 0;

	// Find first object hit by ray
	object *object_hit = NULL;
	int hit = 0;
	while(c < scene->object_count) {
		hit |= Intersect(scene->objects[c], r, &hitrec);
		++c;
	}

	// Determine the color the ray would be 'carrying'
	// Currently this is just based on the normal and it's hard-coded.
	// TODO : add properties to objects that can be called on to determine the color instead.
	if (hit) {
		vec3 n = hitrec.n;
		return color_mul(fcolor_new(n.x + 1.0, n.y + 1.0, n.z + 1.0), 0.5);
	}
	return fcolor_new(0.24, 0.55, 1.0);
}

void Render(fcolor *pixels, int samples, int height, int width, point3 origin, point3 vp_corner, vec3 horizontal, vec3 vertical, scene *scene) {
	int i, j;
	for (j = height-1; j >= 0; --j) {
		for (i = 0; i < width; ++i) {
			int s;
			for (s = 0; s < samples; ++s) {
				float u = ((float)i + random_float()) / (float)(width-1);
				float v = ((float)j + random_float()) / (float)(height-1);

				vec3 d = vec3_sub(vec3_add(vec3_add(vp_corner, vec3_mul(horizontal, u)), vec3_mul(vertical, v)), origin); ray r = {&origin, &d};
				fcolor sample = {0.0, 0.0, 0.0};
				COLOR_ADD(*pixels, TraceRay(&r, scene)); 
			}
			++pixels;
		}
	}
}

int main(int argc, char **argv) {
	struct arguments args = {};

	args.debug_scene = 0;

	// Set default arguments in case they are not given
	args.samples_per_pixel = 10;
	args.image_width = 720;
	args.image_height = 0;
	args.seed = 0;
	argp_parse(&argp, argc, argv, 0, 0, &args);

	if (args.seed != 0) {
		srand(args.seed);
	} else {
		srand(time(NULL)); // Default seed is 1 so this ensures the seed changes each run
	}

	float aspect_ratio;
	if (args.image_height) {
		aspect_ratio = (float)args.image_width / (float)args.image_height;
	} else {
		aspect_ratio = 16.0 / 9.0;
		args.image_height = args.image_width / aspect_ratio;
	}

	int bytes_per_channel = sizeof(char);
	FILE *ppm_file = MakePPMFile(
				args.outfile,
				args.image_height,
				args.image_width
				);

	if (!ppm_file) {
		puts("Could not open given file name.");
		return -1;
	}


	float vp_height = 2.0;
	float vp_width = vp_height * aspect_ratio;
	float focal_length = 1.0;

	point3 origin = {0.0, 0.0, 0.0};
	vec3 vertical = {0.0, 0.0, vp_height};
	vec3 horizontal = {0.0, vp_width, 0.0};
	vec3 vp_corner = vec3_sub(vec3_sub(vec3_sub(origin, vec3_div(horizontal, 2.0)), vec3_div(vertical, 2.0)), vec3_new(focal_length, 0.0, 0.0));


	memory_region mem_region = make_memory_region(256);
	scene s = (args.debug_scene > 0) ? DebugScene(&mem_region) : RandomTestScene(&mem_region);
	fcolor *pixels = malloc(args.image_height * args.image_width * sizeof(fcolor)); 

	Render(pixels, args.samples_per_pixel, args.image_height, args.image_width, origin, vp_corner, horizontal, vertical, &s);

	WriteToPPM(pixels, args.samples_per_pixel, args.image_height, args.image_width, bytes_per_channel, ppm_file);
	fclose(ppm_file);

	free(pixels);
	FreeScene(&s);
	FreeMemoryRegion(&mem_region);

	return 0;
}
