#include "process.h"
#include "string.h"
#include "error.h"
#include "stdio.h"

error_t scaleRgb(Image* const image, uint8_t scale_red, uint8_t scale_green, uint8_t scale_blue) {
	if (image == NULL) return NULL_IMAGE;
	if (image->num_components != 3) return INVALID_IMAGE_RGB;

	if (scale_red < 100) {
		printf("Scaling red colour plane to %d%%\n", scale_red);
		error_t err_code = scaleComponent(image, colour_red, scale_red);
		if (err_code != SUCCESS) return err_code;
	}
	if (scale_green < 100) {
		printf("Scaling green colour plane to %d%%\n", scale_green);
		error_t err_code = scaleComponent(image, colour_green, scale_green);
		if (err_code != SUCCESS) return err_code;
	}
	if (scale_blue < 100) {
		printf("Scaling blue colour plane to %d%%\n", scale_blue);
		error_t err_code = scaleComponent(image, colour_blue, scale_blue);
		if (err_code != SUCCESS) return err_code;
	}

	return SUCCESS;
}

error_t scaleComponent(Image* const image, const colour_t colour, uint8_t scale) {
	if (image == NULL) return NULL_IMAGE;
	if (image->components == NULL) return NULL_COMPONENT;

	if (colour != colour_green && colour != colour_blue && colour != colour_red) return INVALID_COMMAND;

	if (scale > 100) return INVALID_SCALE;

	size_t width = (size_t)image->components[colour].width;
	size_t height = (size_t)image->components[colour].height;

	for (size_t j = 0; j < width * height; ++j) {
		uint16_t value = image->components[colour].data[j];	// Larger data type for intermediate calculation
		value = (value * scale + 50) / 100;					// + 50 is for rounding
		image->components[colour].data[j] = (uint8_t)value;	// This is safe from overflow since 0 <= value <= 255
	}

	return SUCCESS;
}