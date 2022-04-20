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

#include "output.h"

#include "timer.h"

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
	int scattered = Scatter(hitrec.mat, hitrec.hit_front, &hitrec.n, &scattered_ray);

	if (scattered == 0) return fcolor_new(0.0, 0.0, 0.0);

	// NOTE : assign this to a variable for debugging purposes
	fcolor recursive_result = TraceRay(scattered_ray, scene, bgcolor, calldepth - 1);
	COLOR_MUL(recursive_result, TextureColor(hitrec.text, hitrec.u, hitrec.v, hitrec.pt));
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
	args.jpeg_quality = 100;

	if (argp_parse(&argp, argc, argv, 0, 0, &args)) {
		puts("Error parsing arguments. Use -? for help.");
		return -1;
	}

	enum FileExtension output_extension = GetImageType(args.outfile);
	if (output_extension == ERROR) {
		printf("Error -- cannot handle given filetype.");
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

	memory_region mem_region = make_memory_region(4096);
	scene s = SceneSelect(&mem_region, args.scene);

	// NOTE : undo the || eventually
	if (args.verbose || 1) printf("Memory region use: %d / %d bytes.\n", mem_region.used, mem_region.max_size);

	fcolor *pixels = malloc(args.img_height * args.img_width * sizeof(fcolor)); 

	timer render_timer = {};
	TIMER_START(render_timer);

	Render(pixels, args.samples, args.img_height, args.img_width, args.max_depth, origin, vp_corner, horizontal, vertical, &s);
	TIMER_END(render_timer);


	uint8_t *uint8_t_pixels = PixelToUInt8(pixels, args.samples, args.img_height, args.img_width, bytes_per_channel);

	free(pixels); // no longer need float pixels

	int saved = SaveRenderToImage(args.outfile, uint8_t_pixels, args.img_height, args.img_width, output_extension, args.jpeg_quality);

	free(uint8_t_pixels);
	FreeScene(&s);
	FreeMemoryRegion(&mem_region);

	TIMER_END(runtime);
		
	printf("Render Time: %fs\n", TIMER_DURATION_S(render_timer));
	printf("Total Time: %fs\n", TIMER_DURATION_S(runtime));

	if (saved) {
		puts("Image saved successfully.");
	} else {
		puts("ERROR: failed to write output image!");
		return -1;
	}

	return 0;
}
