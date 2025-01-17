#include "process.h"
#include "string.h"
#include "error.h"


int processImage(Image* const image, const char* const command) {
	if (strncmp(command, "isolate-", 8) == 0) return isolateComponents(image, command + 8);
	return INVALID_COMMAND;
}

int isolateComponents(Image* const image, const char* colours) {
	if (image->num_components != 3) return ISOLATE_GREYSCALE;
	if (*colours == '\0') return INVALID_COMMAND_ISOLATE;

	int r = 0, g = 0, b = 0;

	while (*colours != '\0') {
		if (*colours == 'r') r++;
		else if (*colours == 'g') g++;
		else if (*colours == 'b') b++;
		else return INVALID_COMMAND_ISOLATE;
		colours++;
	}

	if (*colours != '\0' || r > 1 || g > 1 || b > 1) return INVALID_COMMAND_ISOLATE;

	if (b == 0) {
		int err_code = zeroComponent(image, colour_blue);
		if (err_code != 0) return err_code;
	}
	if (g == 0) {
		int err_code = zeroComponent(image, colour_green);
		if (err_code != 0) return err_code;
	}
	if (r == 0) {
		int err_code = zeroComponent(image, colour_red);
		if (err_code != 0) return err_code;
	}

	return SUCCESS;
}

int zeroComponent(Image* const image, const colour_t colour) {
	if (image == NULL) return NULL_IMAGE;
	if (image->components == NULL) return NULL_COMPONENT;

	size_t width = (size_t)image->components[colour].width;
	size_t height = (size_t)image->components[colour].height;

	for (size_t j = 0; j < width * height; ++j) {
		image->components[colour].data[j] = 0;
	}

	return SUCCESS;
}