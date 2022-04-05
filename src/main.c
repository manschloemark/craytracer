/* main.c ray tracer
 * A very simple minimalistic raytracer that I hope to incrementally work on.
 * Mark Schloeman
 * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <math.h>

#include "argparse.h"

/* Data Types */

struct Vector3 {
	float x, y, z;
};
typedef struct Vector3 vec3;

struct Color {
	float red, green, blue;
};
typedef struct Color color;

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
	// This appears to be slower than mallocing a new array of unsigned chars. It's not like this is important but still.
	//int count;
	//count = 0;
	//unsigned char p[3];
	//while (count < (height * width)) {
	//	p[0] = (unsigned char)(Clamp(pixels[count].red, 0.0, 0.999) * 256);
	//	p[1] = (unsigned char)(Clamp(pixels[count].green, 0.0, 0.999) * 256);
	//	p[2] = (unsigned char)(Clamp(pixels[count].blue, 0.0, 0.999) * 256);
	//	fwrite(p, 1, 3, dest);
	//	++count;
	//}
	unsigned char *temp = malloc(height * width * bytes_per_channel * 3);
	int count;
	for (count = 0; count < height * width * 3; count = count + 3) {
		temp[count] = (unsigned char)(Clamp(pixels->red, 0.0, 0.999) * 256);
		temp[count+1] = (unsigned char)(Clamp(pixels->green, 0.0, 0.999) * 256);
		temp[count+2] = (unsigned char)(Clamp(pixels->blue, 0.0, 0.999) * 256);
		++pixels;
	}
	fwrite(temp, height * width * 3, bytes_per_channel, dest);
	fflush(dest);
	free(temp);
}
/* Rendering */

void GetColor(color *c, float u, float v) {
	float r = sqrtf(u*u+v*v);
	if (r < 0.16) {
		c->red = 1.0 * fabs(u);
		c->green = 1.0 * fabs(u);
		c->blue = 1.0 * fabs(v * u);
	} else {
		c->red = fmod(u, 0.1);
		c->green = fmod(u, 0.05);
		c->blue = sin(u*v);
	}
}

void PutColor(unsigned char *pixel, color *c) {
	return;
}

void Render(color *pixels, int height, int width, int bytes_per_channel) {
	int i, j;
	float x, y;
	int scale = Max(height, width);
	for (j = 0; j < height; ++j) {
		for (i = 0; i < width; ++i) {
			x = -1.0 + (2.0*((float)i / (float)width));
			y = -1.0 + (2.0*((float)j / (float)height));
			x *= ((float)width / (float)scale);
			y *= ((float)height / (float)scale);

			GetColor(pixels, x, y); 
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

	color *pixels = malloc(args.image_height * args.image_width * sizeof(color)); 
	Render(pixels, args.image_height, args.image_width, bytes_per_channel);
	WriteToPPM(pixels, args.image_height, args.image_width, bytes_per_channel, ppm_file);

	free(pixels);
	fclose(ppm_file);

	return 0;
}
