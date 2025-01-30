#ifndef PROCESS_H
#define PROCESS_H

#include "image.h"

typedef enum {
	colour_blue = 0,
	colour_green = 1,
	colour_red = 2
} Colour;

typedef struct {
	int radius;
	float* data;
} Filter;

// Scales (multiplies) each pixel value of each colour component by its respective scaling factor
Error scaleRgb(Image* const image, uint8_t scale_red, uint8_t scale_green, uint8_t scale_blue);

// Scales (multiplies) each pixel value of a single colour component by `scale`
Error scaleImageComp(Image* const image, const Colour colour, uint8_t scale);

// Allocates memory for a Filter object
Error initFilter(Filter** filter);

// Frees memory used by a Filter object
void freeFilter(Filter* const filter);

// Parses a filter from a file
Error parseFilter(Filter* const filter, const char* const filepath);

// Applies a filter to an image
Error applyFilter(Image* const image, const Filter* const filter);

// Applies a filter to a single colour component
Error applyFilterComp(ImageComp* const image_comp, const Filter* const filter);


#endif // PROCESS_H