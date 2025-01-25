#ifndef PROCESS_H
#define PROCESS_H

#include "image.h"

typedef enum {
	colour_blue = 0,
	colour_green = 1,
	colour_red = 2
} colour_t;

// Scales (multiplies) each pixel value of each colour component by its respective scaling factor
error_t scaleRgb(Image* const image, uint8_t scale_red, uint8_t scale_green, uint8_t scale_blue);

// Scales (multiplies) each pixel value of a single colour component by `scale`
error_t scaleComponent(Image* const image, const colour_t colour, uint8_t scale);


#endif // PROCESS_H