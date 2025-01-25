#ifndef IMAGE_H
#define IMAGE_H

#include "stdint.h"
#include "error.h"

// A colour plane component containing values for a single colour channel
typedef struct Component {
	int width;
	int x_border;
	int y_border;
	int height;
	uint8_t* image;	// First pixel of image data
	uint8_t* data;	// Start of total data (source pixels + extra pixels for processing)
} Component;

// An image containing multiple colour plane components
typedef struct Image {
	int num_components;
	Component* components;
} Image;

// Allocates memory for an Image object
error_t initImage(Image** image);

// Frees all memory used by an Image object
void freeImage(Image* const image);

// Reads data from a bmp file into an Image object
error_t readBmp(Image* const image, const char* const in_file, int x_border, int y_border);

// Writes data from an Image object to a bmp file
error_t writeBmp(const Image* const image, const char* const out_file);

#endif // IMAGE_H
