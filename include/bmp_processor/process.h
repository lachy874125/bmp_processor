#ifndef PROCESS_H
#define PROCESS_H

#include "image.h"

typedef enum {
	colour_blue = 0,
	colour_green = 1,
	colour_red = 2
} colour_t;

// Executes an image processing algorithm from the supplied command
int processImage(Image* const image, const char* const command);

// Isolates one or more colour components in an image
int isolateComponents(Image* const image, const char* colours);

// Sets all values within a colour component to zero
int zeroComponent(Image* const image, colour_t colour);


#endif // PROCESS_H