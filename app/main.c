#include "io_bmp.h"
#include "image.h"
#include "error.h"
#include "process.h"
#include "string.h"

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} Rgb;

// Parse command-line args for colour-scaling
Error parseRgb(Rgb* const rgb, const char* args) {
	rgb->red = 100;
	rgb->green = 100;
	rgb->blue = 100;

	char* p = (char*)args;
	while (*p != '\0') {
		if (strncmp(p, "r=", 2) == 0) {
			char* end;
			rgb->red = strtol(p + 2, &end, 10);
			p = end;
		} else if (strncmp(p, "g=", 2) == 0) {
			char* end;
			rgb->green = strtol(p + 2, &end, 10);
			p = end;
		} else if (strncmp(p, "b=", 2) == 0) {
			char* end;
			rgb->blue = strtol(p + 2, &end, 10);
			p = end;
		} else {
			return INVALID_COMMAND;
		}
		if (*p == ',') p++;
	}

	return SUCCESS;
}


Error processScaleRgbCommand(Image** image, const char* scale_args, const char* input_file) {
	Rgb rgb;
	Error err_code = parseRgb(&rgb, scale_args);
	if (err_code != SUCCESS) return err_code;

	// Initialise image
	err_code = initImage(image);
	if (err_code != SUCCESS) return err_code;

	// Read BMP pixel data into Image object
	err_code = readBmp(*image, input_file, 0, 0);
	if (err_code != SUCCESS) return err_code;

	// Process image
	return scaleRgb(*image, rgb.red, rgb.green, rgb.blue);
}


Error processFilterCommand(Image** image, const char* filter_path, const char* input_file) {
	Filter* filter;
	Error err_code = initFilter(&filter);
	if (err_code != SUCCESS) return err_code;

	err_code = parseFilter(filter, filter_path);
	if (err_code != SUCCESS) {
		freeFilter(filter);
		return err_code;
	}

	int x_border = filter->radius;
	int y_border = filter->radius;
		
	// Initialise image
	err_code = initImage(image);
	if (err_code != SUCCESS) {
		freeFilter(filter);
		return err_code;
	}

	// Read BMP pixel data into Image object
	err_code = readBmp(*image, input_file, x_border, y_border);
	if (err_code != SUCCESS) {
		freeFilter(filter);
		return err_code;
	}

	// Process image
	err_code = applyFilter(*image, filter);
	
	freeFilter(filter);

	return err_code;
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

	// Process image based on command
	Image* image;
	Error err_code;
	if (strncmp(command, "scale-rgb:", 10) == 0) {
		err_code = processScaleRgbCommand(&image, command + 10, input_file);		
	} else if (strncmp(command, "filter:", 7) == 0) {
		err_code = processFilterCommand(&image, command + 7, input_file);
	} else {
		err_code = INVALID_COMMAND;
	}

	if (err_code != SUCCESS) {
		printErrorString(err_code);
		freeImage(image);
		return err_code;
	}

	// Write BMP from processed Image object
	err_code = writeBmp(image, output_file);
	if (err_code != SUCCESS) {
		printErrorString(err_code);
	}

	printf("Image processed successfully.\n");

	freeImage(image);

	return 0;
}