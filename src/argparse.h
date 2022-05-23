#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <argp.h>
#include <string.h>
#include <math.h>

const char *argp_program_version = "craytracer 0.1"; // NOTE : I don't really use version numbers. Maybe I should start.
const char *argp_program_bug_address = "<markofwisdumb@gmail.com>";
static char doc[] = "craytracer -- a personal minimal ray tracer written in C.";

// Format of argument options:
// {argument_name, key, input_name, argp_flags, documentation_string, group_number}
// Groups: 1 = output. 2 = render. 3 = performance. 4 = debugging.
static struct argp_option options[] = {
	{"output", 'o', "FILE", 0, "Path where render is stored. Works with .jpg, .png, .ppm, bmp.", 1},
	{"jpeg-quality", 'j', "QUALITY", 0, "Int in [1-100]. Sets of JPEG output. Default is 100.", 1},
	{"width", 'w', "WIDTH", 0, "Width of output image in pixels.", 1},
	{"height", 'h', "HEIGHT", 0, "Height of output image in pixels.", 1},
	{"vfov", 'f', "VFOV", 0, "Float that sets vertical field of view - smaller numbers are more zoomed in.", 1},
	{"scene", 'c', "SCENE", 0, "Which scene to generate -- SCENE is an integer used in a switch statement.", 2},
	{"num-samples", 'n', "N_SAMPLES", 0, "Take a sample from each pixel N_SAMPLES times", 2},
	{"max-depth", 'd', "MAX_DEPTH", 0, "MAX_DEPTH is the number of times a ray can be reflected.", 2},
	{"thread-count", 't', "N_THREADS", 0, "Number of threads to use. (1 is better for renders)", 2},
	{"job-count", 'b', "N_THREADS", 0, "Number of jobs to create. Splits up the image into pieces that are rendered by threads.", 2},
	{"seed", 's', "SEED", 0, "Seed to pass to stdlib.h srand()", 2},
	//{"num-threads", 't', "N_THREADS", 0, "Create N_THREADS threads to render the image in parallel. Default is estimated number of cores.", 2},
	// Debugging related
	{"verbose", 'v', 0, OPTION_NO_USAGE, "Verbose output. Prints extra info while rendering.", 4},
	// TODO :: implement proper logging capability.
	// {"logfile",         'l', "LOGFILE", OPTION_HIDDEN | OPTION_ARG_OPTIONAL, "The file to which log messages will be sent", 4},
	{0} // This needs to be here to argp knows where the options list ends.
};

struct arguments {
	//int scene;
	char *outfile;
	int jpeg_quality;
	int img_width, img_height;
	int samples, max_depth, num_threads;
	int verbose;
	unsigned int seed;
	int scene;
	int threadcount, jobcount;
	float vfov;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *args = (struct arguments *)state->input;

	switch (key)
	{
	case 'o':
		args->outfile = arg;
		break;
	case 'j':
		args->jpeg_quality = atoi(arg);
		break;
	case 'w':
		args->img_width = atoi(arg);
		break;
	case 'h':
		args->img_height = atoi(arg);
		break;
	case 'f':
		args->vfov = atof(arg);
		break;
	case 'n':
		args->samples = atoi(arg);
		break;
	case 'c':
		args->scene = atoi(arg);
		break;
	case 'd':
		args->max_depth = atoi(arg);
		break;
	case 's':
		args->seed = (unsigned int)atoi(arg);
		break;
	case 't':
		args->threadcount = atoi(arg);
		break;
	case 'b': // TODO : maybe rename to bandcount
		args->jobcount = atoi(arg);
		break;
	/*
	case 't':
		args->num_threads = atoi(arg);
		break;
	*/
	case 'v':
		args->verbose = 1;
		break;
	default:
		return ARGP_ERR_UNKNOWN;
		break;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, 0, doc};

#endif
