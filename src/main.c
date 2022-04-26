/* main.c ray tracer
 * A very simple minimalistic raytracer that I will word on incrementally.
 * Mark Schloeman
 * */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "argparse.h"

#include "common.h"
#include "color.h"
#include "ray.h"
#include "camera.h"

#include "objects.h"
#include "material.h"
#include "texture.h"
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

 // NOTE : if the ray does not scatter the material is diffuse light
 // if it hit the back of a light I don't want it to emit light, so return black instead
	if (scattered == 0)
		return (hitrec.hit_front) ? TextureColor(hitrec.text, hitrec.u, hitrec.v, hitrec.pt) : COLOR_BLACK;

	// NOTE : assign this to a variable for debugging purposes
	fcolor recursive_result = TraceRay(scattered_ray, scene, bgcolor, calldepth - 1);
	COLOR_MUL(recursive_result, TextureColor(hitrec.text, hitrec.u, hitrec.v, hitrec.pt));
	return recursive_result;
}

void Render(fcolor *pixels, int samples, int height, int width, int max_depth, camera *cam, scene *scene) {
	fcolor bgcolor = fcolor_new(0.0, 0.0, 0.0);
	int i, j;
	for (j = height-1; j >= 0; --j) {
		printf("\r%d lines remaining", j);
		for (i = 0; i < width; ++i) {
			int s;
			for (s = 0; s < samples; ++s) {
				float u = ((float)i + random_float()) / (float)(width-1);
				float v = ((float)j + random_float()) / (float)(height-1);

				ray r = camera_cast_ray(cam, u, v);
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
	timer render_timer = {};
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

	int bytes_per_channel = sizeof(uint8_t);


	point3 origin = vec3_new(0.0, 0.0, 10.0);
	point3 target = vec3_new(0.0, 0.0, 0.0);

	memory_region mem_region = make_memory_region(MEGABYTES(1));
	scene s = {};
	SceneSelect(&mem_region, args.scene, &s, &origin, &target);

	//point3 origin = vec3_new(0.0, 0.0, 0.0);
	vec3 vup = {0.0, 0.0, 1.0};
	float vfov = 90.0;
	float focal_length = vec3_len(vec3_sub(target, origin));
	camera cam = make_camera(origin, target, vup, vfov, aspect_ratio, focal_length);


	// NOTE : undo the || eventually
	if (args.verbose || 1) printf("Memory region use: %d / %d bytes.\n", mem_region.used, mem_region.max_size);

	fcolor *pixels = malloc(args.img_height * args.img_width * sizeof(fcolor)); 

	TIMER_START(render_timer);
	Render(pixels, args.samples, args.img_height, args.img_width, args.max_depth, &cam, &s);
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
