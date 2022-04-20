#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stb_image_include.h"

#include "common.h"
#include "color.h"

// NOTE : maybe I should require the length of the filename?
// I know it would be wise but I don't know if I need to because the string comes straight from argv

enum FileExtension {
	JPEG, PPM, PNG, BMP, ERROR
};

// From stack overflow
const char *GetFileExtension(const char *filename) {
	const char *c = filename;

	while(c) {
		if (*c == '.')
			break;
		++c;
	}

	if (!c || c == filename) return NULL;
	return c + 1;
}

enum FileExtension GetImageType(const char *filename) {
	const char *file_extension = GetFileExtension(filename);

	switch (*file_extension) {
		case 'j':
			return (!strcmp(file_extension+1, "pg")) ? JPEG : ERROR;
		case 'p':
			return (!strcmp(file_extension+1, "pm")) ? PPM : (!strcmp(file_extension+1, "ng")) ? PNG : ERROR;
		case 'b':
			return (!strcmp(file_extension+1, "mp")) ? BMP : ERROR;
		default:
			return ERROR;
	}
}

void GetUniqueFilename(char *filename) {
	return;
}

void *PixelToUInt8(fcolor *pixels, int samples, int height, int width, int bytes_per_channel) {
	uint8_t *temp = (uint8_t *)malloc(height * width * bytes_per_channel * 3);
	int count;
	for (count = 0; count < height * width * 3; count = count + 3) {
		// gamma 2 correction
		float r = sqrt(pixels->r / (float)samples);
		float g = sqrt(pixels->g / (float)samples);
		float b = sqrt(pixels->b / (float)samples);
		temp[count] = (uint8_t)(clamp(r, 0.0, 0.999) * 256);
		temp[count+1] = (uint8_t)(clamp(g, 0.0, 0.999) * 256);
		temp[count+2] = (uint8_t)(clamp(b, 0.0, 0.999) * 256);
		++pixels;
	}
	return temp;
}

// Following precedent from stb_image_write, 0 = error, non-0 = success
int WriteToPPM(const char *filename, int height, int width, uint8_t *pixels) {
	FILE *dest = fopen(filename, "w");
	if (!dest) return 0;

	char *header;
	header = (char *)malloc(sizeof(char) * 64);
	snprintf(header, 64, "P6 %i %i %i\n", width, height, 255);
	fputs(header, dest);
	fflush(dest);
	free(header);

	fwrite(pixels, height * width * 3, sizeof(uint8_t), dest);
	fflush(dest);
	fclose(dest);
	return 1;
}

// NOTE : quality is an optional arg only used with .jpg
int SaveRenderToImage(const char *filename, void *pixels, int height, int width, enum FileExtension ext, int quality) {
	int stride_in_bytes;
	int num_channels = 3;
	switch (ext) {
		case PPM:
			return WriteToPPM(filename, height, width, (uint8_t *)pixels);
		case JPEG:
			return stbi_write_jpg(filename, width, height, num_channels, pixels, quality);
		case PNG:
			stride_in_bytes = width * sizeof(uint8_t) * num_channels;
			return stbi_write_png(filename, width, height, num_channels, pixels, stride_in_bytes);
		case BMP:
			return stbi_write_bmp(filename, width, height, num_channels, pixels);
		default:
			return 0;
	}
}


#endif
