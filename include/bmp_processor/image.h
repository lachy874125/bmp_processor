#ifndef IMAGE_H
#define IMAGE_H

#include "stdint.h"
#include "error.h"

// A colour plane component containing values for a single colour channel
typedef struct {
	int width;
	int x_border;
	int height;
	int y_border;
	uint8_t* image;	// First pixel of image data
	uint8_t* data;	// Start of total data (source pixels + extra pixels for processing)
} ImageComp;

// An image containing multiple colour plane components
typedef struct {
	int num_components;
	ImageComp* components;
} Image;

// Allocates memory for an Image object
Error initImage(Image** image);

// Frees all memory used by an Image object
void freeImage(Image* const image);

// Reads data from a bmp file into an Image object
Error readBmp(Image* const image, const char* const in_file, int x_border, int y_border);

// Extends the image boundary by reflecting pixel values across each edge
Error extendBoundary(Image* const image);

// Writes data from an Image object to a bmp file
Error writeBmp(const Image* const image, const char* const out_file);

#endif // IMAGE_H
