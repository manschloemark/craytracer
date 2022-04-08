/* main.c ray tracer
 * A very simple minimalistic raytracer that I will word on incrementally.
 * Mark Schloeman
 * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <math.h>

#include "argparse.h"
#include "vector3.h"
#include "color.h"
#include "ray.h"

#include "objects.h"
/* Data Types */


static inline float Clamp(float n, float min, float max) {
	if (n < min) return min;
	if (n > max) return max;
	return n;
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


void WriteToPPM(color *pixels, int height, int width, int bytes_per_channel, FILE *dest) {
	unsigned char *temp = malloc(height * width * bytes_per_channel * 3);
	int count;
	for (count = 0; count < height * width * 3; count = count + 3) {
		temp[count] = (unsigned char)(Clamp(pixels->r, 0.0, 0.999) * 256);
		temp[count+1] = (unsigned char)(Clamp(pixels->g, 0.0, 0.999) * 256);
		temp[count+2] = (unsigned char)(Clamp(pixels->b, 0.0, 0.999) * 256);
		++pixels;
	}
	fwrite(temp, height * width * 3, bytes_per_channel, dest);
	fflush(dest);
	free(temp);
}
/* Rendering */


color TraceRay(ray *r) {
	color bgcolor = color_new(0.24, 0.33, 1.0);
	// Equation to check sphere intersection
	color sphere_color = color_new(0.66, 0.2, 0.15);

	sphere sp = {};
	sp.center = vec3_new(-10.0, 0.0, 0.0);
	sp.radius = 1.0;

	if (IntersectSphere(&sp, r) > 0) {
		return sphere_color;
	}
	return bgcolor;
}

void Render(color *pixels, int height, int width, point3 origin, point3 vp_corner, vec3 horizontal, vec3 vertical) {
	int i, j;
	for (j = height-1; j >= 0; --j) {
		for (i = 0; i < width; ++i) {
			float u = (float)i / (float)(width-1);
			float v = (float)j / (float)(height-1);

			vec3 d = vec3_sub(vec3_add(vec3_add(vp_corner, vec3_mul(horizontal, u)), vec3_mul(vertical, v)), origin);
			ray r = {&origin, &d};
			color sample = {0.0, 0.0, 0.0};
			COLOR_ADD(*pixels, TraceRay(&r)); 
			++pixels;
		}
	}
}

int main(int argc, char **argv) {
	struct arguments args = {};
	argp_parse(&argp, argc, argv, 0, 0, &args);

	int bytes_per_channel = sizeof(char);
	FILE *ppm_file = MakePPMFile(
				args.outfile,
				args.image_height,
				args.image_width
				);

	if (!ppm_file) return -1;

	float aspect_ratio = (float)args.image_width / (float)args.image_height;

	float vp_height = 2.0;
	float vp_width = vp_height * aspect_ratio;
	float focal_length = 1.0;

	point3 origin = {0.0, 0.0, 0.0};
	vec3 vertical = {0.0, 0.0, vp_height};
	vec3 horizontal = {0.0, vp_width, 0.0};
	vec3 vp_corner = vec3_sub(vec3_sub(vec3_sub(origin, vec3_div(horizontal, 2.0)), vec3_div(vertical, 2.0)), vec3_new(focal_length, 0.0, 0.0));

	color *pixels = malloc(args.image_height * args.image_width * sizeof(color)); 
	Render(pixels, args.image_height, args.image_width, origin, vp_corner, horizontal, vertical);
	WriteToPPM(pixels, args.image_height, args.image_width, bytes_per_channel, ppm_file);
	fclose(ppm_file);
	free(pixels);

	return 0;
}
