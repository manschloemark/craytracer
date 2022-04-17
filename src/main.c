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
#include "material.h"
#include "scene.h"

#include "memory.h"

#include "timer.h"
/* Data Types */

static inline float Clamp(float n, float min, float max) {
	return (n < min) ? min : (n > max) ? max : n;
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
	free(header);
	return newfile;
}


void WriteToPPM(fcolor *pixels, int samples, int height, int width, int bytes_per_channel, FILE *dest) {
	unsigned char *temp = malloc(height * width * bytes_per_channel * 3);
	int count;
	for (count = 0; count < height * width * 3; count = count + 3) {
		// gamma 2 correction
		float r = sqrt(pixels->r / (float)samples);
		float g = sqrt(pixels->g / (float)samples);
		float b = sqrt(pixels->b / (float)samples);
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
fcolor TraceRay(ray r, scene *scene, fcolor *bgcolor, int calldepth) {
	if (calldepth <= 0) return fcolor_new(0.0, 0.0, 0.0);

	hit_record hitrec = {};
	hitrec.t_min = 0.0001; // Prevent rounding errors when t ~~ 0.0;
	hitrec.t = 123123902.0; // TODO : make some const to use for this instead

	int hit = 0;
	int c = 0;
	while(c < scene->object_count) {
		hit |= Intersect(scene->objects[c], &r, &hitrec);
		++c;
	}

	if (!hit) return *bgcolor;

	ray scattered_ray = {};
	scattered_ray.pt = hitrec.pt;
	scattered_ray.dir = r.dir;
	int scattered = Scatter(hitrec.mat, &hitrec.n, &scattered_ray);

	if (scattered == 0) return fcolor_new(0.0, 0.0, 0.0);

	// NOTE : assign this to a variable for debugging purposes
	fcolor recursive_result = TraceRay(scattered_ray, scene, bgcolor, calldepth - 1);
	COLOR_MUL(recursive_result, *hitrec.color);
	return recursive_result;
}

void Render(fcolor *pixels, int samples, int height, int width, int max_depth, point3 origin, point3 vp_corner, vec3 horizontal, vec3 vertical, scene *scene) {
	fcolor bgcolor = fcolor_new(0.55, 0.8, 0.9);
	int i, j;
	for (j = height-1; j >= 0; --j) {
		printf("\r%d lines remaining", j);
		for (i = 0; i < width; ++i) {
			int s;
			for (s = 0; s < samples; ++s) {
				float u = ((float)i + random_float()) / (float)(width-1);
				float v = ((float)j + random_float()) / (float)(height-1);

				vec3 d = vec3_sub(vec3_add(vec3_add(vp_corner, vec3_mul(horizontal, u)), vec3_mul(vertical, v)), origin);
				ray r = {origin, d};
				fcolor sample = TraceRay(r, scene, &bgcolor, max_depth);
				COLOR_ADD(*pixels, sample); 
			}
			++pixels;
		}
	}
	printf("\r");
}


int main(int argc, char **argv) {
	timer runtime = {};
	TIMER_START(runtime);

	struct arguments args = {};
	// Default args
	args.img_width = 1280;
	args.img_height = 0;
	args.samples = 10;
	args.max_depth = 10;
	args.scene = -1;
	args.seed = 0;

	if (argp_parse(&argp, argc, argv, 0, 0, &args)) {
		puts("Error parsing arguments. Use -? for help.");
		return -1;
	}

	if (args.seed != 0) {
		srand(args.seed);
	} else {
		srand(time(NULL)); // Default seed is 1 so this ensures the seed changes each run
	}

	float aspect_ratio;
	if (args.img_height) {
		aspect_ratio = (float)args.img_width / (float)args.img_height;
	} else {
		aspect_ratio = 16.0 / 9.0;
		args.img_height = args.img_width / aspect_ratio;
	}

	int bytes_per_channel = sizeof(char);
	FILE *ppm_file = MakePPMFile(
				args.outfile,
				args.img_height,
				args.img_width
				);

	if (!ppm_file) {
		puts("ERROR: Could not open given file name.");
		return -1;
	}

	float vp_height = 2.0;
	float vp_width = vp_height * aspect_ratio;
	float focal_length = 1.0;

	point3 origin = {0.0, 0.0, 0.0};
	vec3 vertical = {0.0, 0.0, vp_height};
	vec3 horizontal = {0.0, vp_width, 0.0};
	vec3 vp_corner = vec3_sub(
				vec3_sub(
					vec3_sub(origin, vec3_div(horizontal, 2.0)),
					vec3_div(vertical, 2.0)
					),
				vec3_new(focal_length, 0.0, 0.0)
				);

	memory_region mem_region = make_memory_region(1024);
	scene s = SceneSelect(&mem_region, args.scene);

	fcolor *pixels = malloc(args.img_height * args.img_width * sizeof(fcolor)); 

	timer render_timer = {};
	TIMER_START(render_timer);

	Render(pixels, args.samples, args.img_height, args.img_width, args.max_depth, origin, vp_corner, horizontal, vertical, &s);
	TIMER_END(render_timer);

	WriteToPPM(pixels, args.samples, args.img_height, args.img_width, bytes_per_channel, ppm_file);
	fclose(ppm_file);
	free(pixels);
	FreeScene(&s);
	FreeMemoryRegion(&mem_region);

	TIMER_END(runtime);
		
	printf("Render Time: %fs\n", TIMER_DURATION_S(render_timer));
	printf("Total Time: %fs\n", TIMER_DURATION_S(runtime));

	return 0;
}
