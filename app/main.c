#include "io_bmp.h"
#include "image.h"
#include "error.h"
#include "process.h"
#include "string.h"

// Parse command-line args for colour-scaling and call iamge processing functions
error_t bmp_processor_scaleRgb(Image* const image, const char* args) {
	uint8_t scale_red = 100;
	uint8_t scale_green = 100;
	uint8_t scale_blue = 100;

	char* p = (char*)args;
	while (*p != '\0') {
		if (strncmp(p, "r=", 2) == 0) {
			char* end;
			scale_red = strtol(p + 2, &end, 10);
			p = end;
		} else if (strncmp(p, "g=", 2) == 0) {
			char* end;
			scale_green = strtol(p + 2, &end, 10);
			p = end;
		} else if (strncmp(p, "b=", 2) == 0) {
			char* end;
			scale_blue = strtol(p + 2, &end, 10);
			p = end;
		} else {
			return INVALID_COMMAND;
		}
		if (*p == ',') p++;
	}

	return scaleRgb(image, scale_red, scale_green, scale_blue);
}

int main(int argc, char* argv[]) {
	// Handle invalid arguments
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <image processing command> <BMP input file> <BMP output file>\n", argv[0]);
		return -1;
	}

	// Parse arguments
	const char* command = argv[1];
	const char* input_file = argv[2];
	const char* output_file = argv[3];

	// Image processing function pointer
	error_t (*processFunction)(Image* const image, const char* command);

	// Border extension
	int x_border;
	int y_border;

	// Map command to image processing function
	int err_code;
	const char* command_args;
	if (strncmp(command, "scale-rgb:", 10) == 0) {
		x_border = 0;
		y_border = 0;
		command_args = command + 10;
		processFunction = bmp_processor_scaleRgb;

	} else {
		err_code = INVALID_COMMAND;
		error_print(err_code);
		return err_code;
	}

	// Initialise image
	Image* image;
	err_code = initImage(&image);
	if (err_code != 0) {
		error_print(err_code);
		freeImage(image);
		return err_code;
	}

	// Read BMP pixel data into Image object
	err_code = readBmp(image, input_file, x_border, y_border);
	if (err_code != 0) {
		error_print(err_code);
		freeImage(image);
		return err_code;
	}

	// Process image
	err_code = processFunction(image, command_args);
	if (err_code != 0) {
		error_print(err_code);
		freeImage(image);
		return err_code;
	}

	// Write BMP from processed Image object
	err_code = writeBmp(image, output_file);
	if (err_code != 0) {
		error_print(err_code);
	}

	freeImage(image);

	return 0;
}