#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <argp.h>
#include <string.h>
#include <math.h>

const char *argp_program_version = "craytracer 0.1"; // NOTE : I don't really use version numbers. Maybe I should start.
const char *argp_program_bug_address = "<markofwisdumb@gmail.com>";
static char doc[] = "craytracer -- a personal minimal ray tracer written in C.";

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
	{"num-samples", 'n', "N_SAMPLES", 0, "Take a sample from each pixel N_SAMPLES times", 2},
	//{"max-depth", 'd', "MAX_DEPTH", 0, "MAX_DEPTH is the number of times a ray can be reflected.", 2},
	//{"num-threads", 't', "N_THREADS", 0, "Create N_THREADS threads to render the image in parallel. Default is estimated number of cores.", 2},
	// TODO :: should this be a runtime flag or a compile time flag? 
	// Debugging related
	{"verbose", 'v', 0, 0, "Verbose output. Prints extra info while rendering.", 3},
	// TODO :: implement proper logging capability.
	// {"logfile",         'l', 0, 0, "The file to which log messages will be sent", 2},
	{"seed", 's', "SEED", 0, "Seed to pass to stdlib.h srand()"},
	{"debug-scene", 'd', "DEBUG", 0, "1 = use DebugScene which should have an intersection at pixel 0, 0"},
	{0} // This needs to be here to argp knows where the options list ends.
};

struct arguments {
	//int scene;
	char *outfile;
	int img_width, img_height;
	int samples, max_depth, num_threads;
	int verbose;
	unsigned int seed;
	int debug_scene;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *args = (struct arguments *)state->input;

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
		args->img_width = atoi(arg);
		break;
	case 'h':
		args->img_height = atoi(arg);
		break;
	case 'n':
		args->samples = atoi(arg);
		break;
	/*
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
	case 'd':
		args->debug_scene = 1;
		break;
	default:
		return ARGP_ERR_UNKNOWN;
		break;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, 0, doc};

#endif
