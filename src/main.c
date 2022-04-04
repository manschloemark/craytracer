/* main.c ray tracer
 * A very simple minimalistic raytracer that I hope to incrementally work on.
 * Mark Schloeman
 * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <math.h>

/* Data Types */

struct Vector3 {
	float red, green, blue;
};

typedef struct Vector3 vec3;
typedef struct Vector3 color;

static inline float Clamp(float n, float min, float max) {
	if (n < min) return min;
	if (n > max) return max;
	return n;
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
	snprintf(header, 64, "P6 %i %i %i\n", height, width, 255);
	fputs(header, newfile);
	fflush(newfile);
	return newfile;
}

void WriteToPPM(color *pixels, int height, int width, int bytes_per_channel, FILE *dest) {
	unsigned char *temp = malloc(height * width * bytes_per_channel * 3);
	int count;
	for (count = 0; count < height * width * 3; count = count + 3) {
		temp[count] = (unsigned char)(Clamp(pixels->red, 0.0, 0.999) * 256);
		temp[count+1] = (unsigned char)(Clamp(pixels->green, 0.0, 0.999) * 256);
		temp[count+2] = (unsigned char)(Clamp(pixels->blue, 0.0, 0.999) * 256);
		++pixels;
	}
	fwrite(temp, height * width * 3, bytes_per_channel, dest);
	fprintf(dest, "\n");
	fflush(dest);
	free(temp);
}

/* Rendering */

void GetColor(color *c, float u, float v) {
	float r = sqrtf(u*u+v*v);
	if (r < 0.16) {
		c->red = 0.5;
		c->green = 0.0;
		c->blue = 1.0;
	} else {
		c->red = fabs(u*v);
		c->green = fabs(u);
		c->blue = fabs(v);
	}
}

void PutColor(unsigned char *pixel, color *c) {
	return;
}

void Render(color *pixels, int height, int width, int bytes_per_channel) {
	int i, j;
	float x, y;
	for (j = 0; j < height; ++j) {
		for (i = 0; i < width; ++i) {
			x = -1.0 + (2.0*((float)i / (float)width));
			y = -1.0 + (2.0*((float)j / (float)width));
			GetColor(pixels, x, y); 
			++pixels;
		}
	}
}

int main(int argc, char **argv) {
	char *fname;
	if (argc == 2) {
		fname = argv[1];
	} else {
		/* maybe use something like this for auto file duplicates
		int max_filename = 64;
		char *fname = malloc(max_filename);
		strncpy(fname, "test.ppm", max_filename);
		GetUniqueFilename(fname); // NOTE : does nothing atm
		*/
		fname = "default_test.ppm";
	}
	int h = 480;
	int w = 480;
	int bytes_per_channel = sizeof(char);


	FILE *ppm_file = MakePPMFile(fname, h, w);
	if (!ppm_file) return -1;

	color *pixels = malloc(h * w * sizeof(color)); 
	Render(pixels, h, w, bytes_per_channel);
	WriteToPPM(pixels, h, w, bytes_per_channel, ppm_file);

	free(pixels);
	fclose(ppm_file);

	return 0;
}
