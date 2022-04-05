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
	return color_new(0.0, 0.5, 0.0);
}

void Render(color *pixels, int height, int width, point3 origin, point3 vp, vec3 y, vec3 z) {
	int i, j;
	int scale = Max(height, width);
	for (j = 0; j < height; ++j) {
		for (i = 0; i < width; ++i) {
			float u = (float)i / (float)(width-1);
			float v = (float)j / (float)(height-1);

			vec3 d = vec3_sub(vec3_add(vec3_add(vp, vec3_mul(y, u)), vec3_mul(z, v)), origin);

			ray r = {&origin, &d};
			*pixels++ = TraceRay(&r); 
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

	// TODO : impelement vector math and whatever

	color *pixels = malloc(args.image_height * args.image_width * sizeof(color)); 
	Render(pixels, args.image_height, args.image_width, bytes_per_channel);
	WriteToPPM(pixels, args.image_height, args.image_width, bytes_per_channel, ppm_file);

	free(pixels);
	fclose(ppm_file);

	return 0;
}
