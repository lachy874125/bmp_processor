#ifndef IMAGE_H
#define IMAGE_H

#include "stdint.h"

// A colour plane component containing values for a single colour channel
typedef struct Component {
	int width;
	int height;
	uint8_t* data;
} Component;

// An image containing multiple colour plane components
typedef struct Image {
	int num_components;
	Component* components;
} Image;

// Frees all memory used by an Image object
void freeImage(Image* image);

// Reads data from a bmp file into an Image object
int readBmp(const char* in_file, Image* image);

// Writes data from an Image object to a bmp file
int writeBmp(char* out_file, const Image* image);

#endif // IMAGE_H
