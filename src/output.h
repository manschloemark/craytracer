#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "color.h"

// NOTE : maybe I should require the length of the filename?
// I know it would be wise but I don't know if I need to because the string comes straight from argv

enum FileExtension {
	JPEG, PPM, PNG, BMP, ERROR
};

const char *GetFileExtension(const char *filename);

enum FileExtension GetImageType(const char *filename);

void GetUniqueFilename(char *filename);

void *PixelToUInt8(fcolor *pixels, int samples, int height, int width, int bytes_per_channel);

// Following precedent from stb_image_write, 0 = error, non-0 = success
int WriteToPPM(const char *filename, int height, int width, uint8_t *pixels);

// NOTE : quality is an optional arg only used with .jpg
int SaveRenderToImage(const char *filename, void *pixels, int height, int width, enum FileExtension ext, int quality);


#endif
