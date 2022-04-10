#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <argp.h>
#include <string.h>
#include <math.h>

const char *argp_program_version = "weekend-raytracing 0.2.2";
const char *argp_program_bug_address = "<markofwisdumb@gmail.com>";
static char doc[] = "Weekend Raytracing -- A personal ray-tracer based off of Peter Shirley's _Ray Tracing in One Weekend_ book series.";

static struct argp_option options[] = {
	// Output options
	// NOTE: should be .ppm format in this version"
	{"output", 'o', "FILE", 0, "Path where render is stored. Should be PPM!"},
	{"width", 'w', "WIDTH", 0, "Width of output image in pixels.", 0},
	{"height", 'h', "HEIGHT", 0, "Height of output image in pixels.", 0},
	// Render options
	// TODO :: specify scene files insead of hardcoded functions
	//{"scene", 's', "SCENE", 0, "Which scene to generate -- SCENE is an integer used in a switch statement.", 1},
	// Performance related
	//{"num-samples", 'n', "N_SAMPLES", 0, "Take a sample from each pixel N_SAMPLES times", 2},
	//{"max-depth", 'd', "MAX_DEPTH", 0, "MAX_DEPTH is the number of times a ray can be reflected.", 2},
	//{"num-threads", 't', "N_THREADS", 0, "Create N_THREADS threads to render the image in parallel. Default is estimated number of cores.", 2},
	// TODO :: should this be a runtime flag or a compile time flag? 
	// Debugging related
	{"verbose", 'v', 0, 0, "Verbose output. Prints extra info while rendering.", 3},
	// TODO :: implement proper logging capability.
	// {"logfile",         'l', 0, 0, "The file to which log messages will be sent", 2},
	{"seed", 's', "SEED", 0, "Seed to pass to stdlib.h srand()"},
	{0} // This needs to be here to argp knows where the options list ends.
};

struct arguments {
	//int scene;
	char *outfile;
	int image_width, image_height;
	//int samples_per_pixel, max_depth, num_threads;
	int verbose;
	unsigned int seed;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *args = (struct arguments *)state->input;

	// TODO :: some of my options need values -- how do I properly retrieve them? What if they are ints instead of chars? Do I need to parse them?
	switch (key)
	{
	/*
	case 's':
		args->scene = atoi(arg);
		break;
	*/
	case 'o':
		args->outfile = arg;
		break;
	case 'w':
		args->image_width = atoi(arg);
		break;
	case 'h':
		args->image_height = atoi(arg);
		break;
	/*
	case 'n':
		args->samples_per_pixel = atoi(arg);
		break;
	case 'd':
		args->max_depth = atoi(arg);
		break;
	case 't':
		args->num_threads = atoi(arg);
		break;
	*/
	case 'v':
		args->verbose = 1;
		break;
	case 's':
		args->seed = (unsigned int)atoi(arg);
		break;
	default:
		return ARGP_ERR_UNKNOWN;
		break;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, 0, doc};

#endif
