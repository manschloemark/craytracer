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
#include <unistd.h>

#include <pthread.h>

#include "argparse.h"
#include "common.h"
#include "color.h"
#include "ray.h"
#include "camera.h"
#include "objects.h"
#include "material.h"
#include "texture.h"
#include "object_list.h"
#include "scene.h"
#include "memory.h"
#include "output.h"
#include "timer.h"

#include "threadpool.h"

#define DEBUG_BANDS 0

fcolor TraceRay(ray r, scene *scene, fcolor *bgcolor, int maxdepth, int calldepth, thread_context *thread) {
	if (calldepth <= 0) return fcolor_new(0.0, 0.0, 0.0);

	hit_record hitrec = {};
	hitrec.t_min = 0.0002; // Prevent rounding errors when t ~~ 0.0;
	hitrec.t = 123123902.0; // TODO : make some const to use for this instead

	int hit = object_list_intersect(&scene->ol, &r, &hitrec, thread);

	if (!hit) return *bgcolor;

	ray scattered_ray = {};
	scattered_ray.pt = hitrec.pt;
	scattered_ray.dir = r.dir;
	int scattered = hitrec.mat->Scatter(hitrec.mat, hitrec.hit_front, &hitrec.n, &scattered_ray, thread);

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
	fcolor recursive_result = TraceRay(scattered_ray, scene, bgcolor, maxdepth, calldepth - 1, thread);
	COLOR_MUL(recursive_result, hitrec.text->TextureColor(hitrec.text, hitrec.u, hitrec.v, hitrec.pt, &hitrec.n));
	return recursive_result;
}

struct render_info {
	int samples, height, width, max_depth;
	fcolor *bgcolor;
	fcolor *pixels;
	camera *cam;
	scene *scene;
	int *chunks_remaining;
};

void Render(struct render_info *rinfo, thread_context *thread) {
	fcolor *pixels = rinfo->pixels;
	for (int j = rinfo->height-1; j >= 0; --j) {
		printf("Lines remaining: %-6d\r", j);
		for (int i = 0; i < rinfo->width; ++i) {
			int s;
			for (s = 0; s < rinfo->samples; ++s) {
				double u = ((double)i + random_double(&thread->rand_state)) / (double)(rinfo->width-1);
				double v = ((double)j + random_double(&thread->rand_state)) / (double)(rinfo->height-1);

				ray r = camera_cast_ray(rinfo->cam, u, v, thread);
				fcolor sample = TraceRay(r, rinfo->scene, rinfo->bgcolor, rinfo->max_depth, rinfo->max_depth, thread);
				COLOR_ADD(*pixels, sample); 
			}
			++pixels;
		}
	}
	printf("\nDone.\n");
}


struct render_thread {
	struct render_info *rinfo;
	thread_context *context;
	int start_j, end_j;
};

void RenderChunk(void *args) {
	struct render_thread *rthread = (struct render_thread *)args;
	struct render_info *rinfo = rthread->rinfo;
	int pixel_row_offset = (rinfo->height - 1 - rthread->start_j) * rinfo->width;

	// TODO : maybe instead of a macro make this a cli flag like "debug-info" or "debug-image" or something like that.
#if DEBUG_BANDS
	// Render first line of band as a single color so you can see where the each job starts.
	int j = rthread->start_j;
	for (int i = 0; i < rinfo->width; ++i) {
		for (int s = 0; s < rinfo->samples; ++s) {
			double u = ((double)i + random_double(&rthread->context->rand_state)) / (double)(rinfo->width-1);
			double v = ((double)j + random_double(&rthread->context->rand_state)) / (double)(rinfo->height-1);

			ray r = camera_cast_ray(rinfo->cam, u, v, rthread->context);
			fcolor sample = COLOR_UNDEFPURP;

			COLOR_ADD(rinfo->pixels[pixel_row_offset + i], sample); 
		}
	}
	pixel_row_offset += rinfo->width;
	for (int j = rthread->start_j - 1; j > rthread->end_j; --j) {
#else
	for (int j = rthread->start_j; j > rthread->end_j; --j) {
#endif
		for (int i = 0; i < rinfo->width; ++i) {
			for (int s = 0; s < rinfo->samples; ++s) {
				double u = ((double)i + random_double(&rthread->context->rand_state)) / (double)(rinfo->width-1);
				double v = ((double)j + random_double(&rthread->context->rand_state)) / (double)(rinfo->height-1);

				ray r = camera_cast_ray(rinfo->cam, u, v, rthread->context);
				fcolor sample;
					sample = TraceRay(r, rinfo->scene, rinfo->bgcolor, rinfo->max_depth, rinfo->max_depth, rthread->context);

				COLOR_ADD(rinfo->pixels[pixel_row_offset + i], sample); 
			}
		}
		pixel_row_offset += rinfo->width;
	}
	*rinfo->chunks_remaining -= 1;
	printf("Bands remaining: %-6d\r", *rinfo->chunks_remaining);
}

void MultithreadRender(struct render_info *rinfo, threadpool *pool, thread_context *contexts, int job_count) {

	int chunk_height = rinfo->height / job_count;
	int j;
	int c = 0;
	// TODO : fix band allocation to make one band per thread, making one band LARGER when image_height % thread_count != 0
	int bands = (int)(ceilf((double)rinfo->height / (double)chunk_height));
	*rinfo->chunks_remaining = bands;
	struct render_thread *rthreads = calloc(bands, sizeof(struct render_thread));
	printf("Render Thread memory use: %d\n", bands * (int)sizeof(struct render_thread));
	int i = 0;
	for (j = rinfo->height-1; j >= 0; j -= (chunk_height)) {
		rthreads[i].rinfo = rinfo;
		rthreads[i].start_j = j;
		rthreads[i].end_j = (rthreads[i].start_j - chunk_height <= 0)
					? -1
					: (rthreads[i].start_j - chunk_height);
		// TODO : fix band allocation to make one band per thread, making one band LARGER when image_height % thread_count != 0
		rthreads[i].context = contexts + (i % job_count);
		int added = threadpool_add_job(pool, RenderChunk, &rthreads[i]);
		++i;
	}
 	// NOTE : my threadpool seems to be slow to pick up jobs so I have to wait and left
	//        the threads in threadpool_worker() take jobs
	//        theres gotta be a better way to do this. Probably just make another
	//        pthread_cond but idk it seems like theres a better way
	printf("Bands remaining: %-6d\r", bands);
	if(pool->active_count == 0) {
		usleep(1000);
	}
	threadpool_wait(pool);
	free(rthreads);
	printf("\nDone.\n");
}


int main(int argc, char **argv) {
	thread_context mainthread = {};
	struct drand48_data buff;
	mainthread.id = 0;
	mainthread.rand_state = buff;

	timer runtime = timer_new();
	timer render_timer = timer_new();
	TIMER_START(runtime);

	struct arguments args = {};
	args.outfile = NULL;
	args.verbose = 0;
	args.img_width = 1280;
	args.img_height = 0;
	args.samples = 10;
	args.max_depth = 10;
	args.scene = -1;
	args.seed = 0;
	args.jpeg_quality = 100;
	args.vfov = 60.0;
	args.threadcount = 4;
	args.jobcount = 12;

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
		//srand(args.seed);
	} else {
		args.seed = time(NULL);
		//srand(args.seed); // seed is always 1 by default. I use current time for variety.
		printf("Random seed: %d\n", args.seed);
	}
	srand48_r(args.seed, &mainthread.rand_state);

	double aspect_ratio;
	if (args.img_height) {
		aspect_ratio = (double)args.img_width / (double)args.img_height;
	} else {
		aspect_ratio = 16.0 / 9.0;
		args.img_height = args.img_width / aspect_ratio;
	}

	int bytes_per_channel = sizeof(uint8_t);

	memory_region mem_region = make_memory_region(MEGABYTES(1));

	point3 origin = vec3_new(0.0, 0.0, 10.0);
	point3 target = vec3_new(0.0, 0.0, 0.0);
	vec3 vup = vec3_new(0.0, 0.0, 1.0);
	scene s = {};
	SceneSelect(&mem_region, args.scene, &s, &origin, &target, &vup, &mainthread);
	// By default, set bgcolor to something light. Then check the scene to see if it has any light objects
	// If there are lights in the scene, set the background color to black so the lights are the only source of light.
	fcolor bgcolor = COLOR_VALUE(0.5);
	for (int obj_index = 0; obj_index < s.object_count; ++obj_index) {
		if (s.objects[obj_index]->mat->id == DiffuseLight) {
			bgcolor = COLOR_BLACK;
			break;
		}
	}

	//point3 origin = vec3_new(0.0, 0.0, 0.0);
	double focal_length = vec3_len(vec3_sub(target, origin));
	camera cam = make_camera(origin, target, vup, args.vfov, aspect_ratio, focal_length);

	// TODO : undo the || eventually
	if (args.verbose || 1)
		printf("Memory region use: %d / %d bytes.\n", mem_region.used, mem_region.max_size);

	fcolor *pixels = calloc(args.img_height * args.img_width, sizeof(fcolor)); 

	struct render_info render_args = {};
	render_args.height = args.img_height;
	render_args.width = args.img_width;
	render_args.samples = args.samples;
	render_args.max_depth = args.max_depth;
	render_args.bgcolor = &bgcolor;
	render_args.pixels = pixels;
	render_args.cam = &cam;
	render_args.scene = &s;

	int chunks = 0;
	render_args.chunks_remaining = &chunks;

	TIMER_START(render_timer);
	threadpool *pool = NULL;
	if(args.threadcount == 1)
		Render(&render_args, &mainthread);
	else {
		setbuf(stdout, NULL); // So progress is displayed in shell
		// make a thread context for each job
		// NOTE : now that I updated thread contexts to be created for each JOB not thread, maybe it's a misnomer.
		// the purpose is still the same. to allow for reentrant rng.
		if (args.jobcount < args.threadcount) args.jobcount = args.threadcount;
		thread_context *contexts = calloc(args.jobcount, sizeof(thread_context));
		for (int i = 0; i < args.jobcount; ++i) {
			contexts[i].id = i+1;
			struct drand48_data buff = {};
			srand48_r(args.seed + contexts[i].id, &buff);
			contexts[i].rand_state = buff;
		}
		pool = threadpool_new(args.threadcount);
		MultithreadRender(&render_args, pool, contexts, args.jobcount);
		free(contexts);
	}
	TIMER_END(render_timer);


	uint8_t *uint8_t_pixels = PixelToUInt8(pixels, args.samples, args.img_height, args.img_width, bytes_per_channel);

	free(pixels); // no longer need double pixels

	int saved = SaveRenderToImage(args.outfile, uint8_t_pixels, args.img_height, args.img_width, output_extension, args.jpeg_quality);

	free(uint8_t_pixels);
	FreeScene(&s);
	FreeMemoryRegion(&mem_region);

	threadpool_free(pool);

	TIMER_END(runtime);
		
	printf("Render Time: %ds\n", TIMER_DURATION_S(render_timer));
	printf("Total Time: %ds\n", TIMER_DURATION_S(runtime));
	timer_free(&render_timer);
	timer_free(&runtime);

	if (saved) {
		puts("Image saved successfully.");
	} else {
		puts("ERROR: failed to write output image!");
		return -1;
	}

	return 0;
}
