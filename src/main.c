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

#include <pthread.h>

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

#include "threadpool.h"

fcolor TraceRay(ray r, scene *scene, fcolor *bgcolor, int maxdepth, int calldepth) {
	if (calldepth <= 0) return fcolor_new(0.0, 0.0, 0.0);

	hit_record hitrec = {};
	hitrec.t_min = 0.0001; // Prevent rounding errors when t ~~ 0.0;
	hitrec.t = 123123902.0; // TODO : make some const to use for this instead

	int hit = 0;
	int c = 0;
	while(c < scene->object_count) {
		hit |= scene->objects[c]->Intersect(scene->objects[c], &r, &hitrec);
		++c;
	}

	if (!hit) return *bgcolor;

	ray scattered_ray = {};
	scattered_ray.pt = hitrec.pt;
	scattered_ray.dir = r.dir;
	int scattered = Scatter(hitrec.mat, hitrec.hit_front, &hitrec.n, &scattered_ray);

 // NOTE : if the ray does not scatter the material is diffuse light
 // if it hit the back of a light I don't want it to emit light, so return black instead
	if (scattered == 0) {
		// NOTE : since lights tend to have really large color values (greater than 1)
		//        I think that is making anti-aliasing less effective on them.
		//        Normalize light colors when it is the first thing a ray hits
		//        that way the light colors won't dominate the colors around them
		if (hitrec.hit_front) {
			if (calldepth == maxdepth) {
				return color_normalize(hitrec.text->TextureColor(hitrec.text, hitrec.u, hitrec.v, hitrec.pt, &hitrec.n));
			}
				return hitrec.text->TextureColor(hitrec.text, hitrec.u, hitrec.v, hitrec.pt, &hitrec.n);
		}
		return COLOR_BLACK;
	}

	// NOTE : I only assign this to a variable for debugging purposes
	fcolor recursive_result = TraceRay(scattered_ray, scene, bgcolor, maxdepth, calldepth - 1);
	COLOR_MUL(recursive_result, hitrec.text->TextureColor(hitrec.text, hitrec.u, hitrec.v, hitrec.pt, &hitrec.n));
	return recursive_result;
}

void Render(fcolor *pixels, int samples, int height, int width, int max_depth, camera *cam, scene *scene) {
	// By default, set bgcolor to something light. Then check the scene to see if it has any light objects
	// If there are lights in the scene, set the background color to black so the lights are the only source of light.
	fcolor bgcolor = COLOR_VALUE(0.5);
	for (int obj_index = 0; obj_index < scene->object_count; ++obj_index) {
		if (scene->objects[obj_index]->mat->id == DiffuseLight) {
			bgcolor = COLOR_BLACK;
			break;
		}
	}
	int i, j;
	for (j = height-1; j >= 0; --j) {
		printf("Lines remaining: %-6d\r", j);
		for (i = 0; i < width; ++i) {
			int s;
			for (s = 0; s < samples; ++s) {
				float u = ((float)i + random_float()) / (float)(width-1);
				float v = ((float)j + random_float()) / (float)(height-1);

				ray r = camera_cast_ray(cam, u, v);
				fcolor sample = TraceRay(r, scene, &bgcolor, max_depth, max_depth);
				COLOR_ADD(*pixels, sample); 
			}
			++pixels;
		}
	}
}

struct render_info {
	int samples, height, width, max_depth;
	fcolor *bgcolor;
	fcolor *pixels;
	camera *cam;
	scene *scene;
};

struct render_thread {
	struct render_info *rinfo;
	int start_i, start_j;
	int chunk_width, chunk_height, end_i, end_j;
};

void RenderChunk(void *args) {
	struct render_thread *rthread = (struct render_thread *)args;
	struct render_info *rinfo = rthread->rinfo;
	int pixel_row_offset = (rinfo->height - 1 - rthread->start_j) * rinfo->width;
	for (int j = rthread->start_j; j > rthread->end_j; --j) {
		for (int i = rthread->start_i; i < rthread->end_i; ++i) {
			for (int s = 0; s < rinfo->samples; ++s) {
				float u = ((float)i + random_float()) / (float)(rinfo->width-1);
				float v = ((float)j + random_float()) / (float)(rinfo->height-1);

				ray r = camera_cast_ray(rinfo->cam, u, v);
				fcolor sample = TraceRay(r, rinfo->scene, rinfo->bgcolor, rinfo->max_depth, rinfo->max_depth);

				COLOR_ADD(rinfo->pixels[pixel_row_offset + i], sample); 
			}
		}
		pixel_row_offset += rinfo->width;
	}
}

void MultithreadRender(int thread_count, fcolor *pixels, int samples, int height, int width, int max_depth, camera *cam, scene *scene, threadpool *pool) {
	// By default, set bgcolor to something light. Then check the scene to see if it has any light objects
	// If there are lights in the scene, set the background color to black so the lights are the only source of light.
	fcolor bgcolor = COLOR_VALUE(0.5);
	for (int obj_index = 0; obj_index < scene->object_count; ++obj_index) {
		if (scene->objects[obj_index]->mat->id == DiffuseLight) {
			bgcolor = COLOR_BLACK;
			break;
		}
	}

	struct render_info rinfo = {};
	rinfo.samples = samples;
	rinfo.height = height;
	rinfo.width = width;
	rinfo.max_depth = max_depth;
	rinfo.cam = cam;
	rinfo.scene = scene;
	rinfo.pixels = pixels;
	rinfo.bgcolor = &bgcolor;

	int chunk_height = 64;
	int chunk_width = 64;
	int i, j;
	int c = 0;
	int i_chunks = (int)(ceilf((float)width / (float)chunk_width));
	int j_chunks = (int)(ceilf((float)height / (float)chunk_height));
	int chunk_count = i_chunks * j_chunks;	
	struct render_thread *rthread_ptr = calloc(chunk_count, sizeof(struct render_thread));
	struct render_thread *rthreads = rthread_ptr;
	printf("Starting multithread render. %d total chunks.\n", (chunk_count));
	for (j = height-1; j >= 0; j -=(chunk_height)) {
		for (i = 0; i < width; i += chunk_width) {
			struct render_thread rthread = {};
			rthread.rinfo = &rinfo;
			rthread.start_i = i;
			rthread.start_j = j;
			rthread.chunk_width = chunk_width;
			rthread.chunk_height = chunk_height;
			int end_i1 = (rthread.start_i + rthread.chunk_width >= width) ? width : (rthread.start_i + rthread.chunk_width);
			int end_j1 = (rthread.start_j - rthread.chunk_height <= 0) ? -1 : (rthread.start_j - rthread.chunk_height);
			rthread.end_i = end_i1;
			rthread.end_j = end_j1;
			*rthreads = rthread;

			threadpool_add_job(pool, RenderChunk, rthreads);
			++rthreads;
		}
	}
	printf("Gonna go wait now.\n");
	threadpool_wait(pool);
	free(rthread_ptr);
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
	args.vfov = 60.0;

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
		int seed = time(NULL);
		srand(seed); // seed is always 1 by default. I use current time for variety.
		printf("Random seed: %d\n", seed);
	}

	float aspect_ratio;
	if (args.img_height) {
		aspect_ratio = (float)args.img_width / (float)args.img_height;
	} else {
		aspect_ratio = 16.0 / 9.0;
		args.img_height = args.img_width / aspect_ratio;
	}

	int bytes_per_channel = sizeof(uint8_t);

	memory_region mem_region = make_memory_region(MEGABYTES(1));

	point3 origin = vec3_new(0.0, 0.0, 10.0);
	point3 target = vec3_new(0.0, 0.0, 0.0);
	scene s = {};
	SceneSelect(&mem_region, args.scene, &s, &origin, &target);

	//point3 origin = vec3_new(0.0, 0.0, 0.0);
	vec3 vup = {0.0, 0.0, 1.0};
	float focal_length = vec3_len(vec3_sub(target, origin));
	camera cam = make_camera(origin, target, vup, args.vfov, aspect_ratio, focal_length);


	// NOTE : undo the || eventually
	if (args.verbose || 1) printf("Memory region use: %d / %d bytes.\n", mem_region.used, mem_region.max_size);

	fcolor *pixels = malloc(args.img_height * args.img_width * sizeof(fcolor)); 

	threadpool *pool = threadpool_new(13);
	TIMER_START(render_timer);
	MultithreadRender(2, pixels, args.samples, args.img_height, args.img_width, args.max_depth, &cam, &s, pool);
	TIMER_END(render_timer);
	threadpool_free(pool);


	uint8_t *uint8_t_pixels = PixelToUInt8(pixels, args.samples, args.img_height, args.img_width, bytes_per_channel);

	free(pixels); // no longer need float pixels

	int saved = SaveRenderToImage(args.outfile, uint8_t_pixels, args.img_height, args.img_width, output_extension, args.jpeg_quality);

	free(uint8_t_pixels);
	FreeScene(&s);
	FreeMemoryRegion(&mem_region);

	TIMER_END(runtime);
		
	printf("Render Time: %ds\n", TIMER_DURATION_S(render_timer));
	printf("Total Time: %ds\n", TIMER_DURATION_S(runtime));

	if (saved) {
		puts("Image saved successfully.");
	} else {
		puts("ERROR: failed to write output image!");
		return -1;
	}

	return 0;
}
