#include "io_bmp.h"
#include "image.h"
#include "error.h"
#include "process.h"

int main(int argc, char* argv[]) {
	// Handle invalid arguments
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <bmp input file> <bmp output file> <image processing command>\n", argv[0]);
		return -1;
	}

	// Read bmp into Image object
	Image image;
	int err_code = readBmp(argv[2], &image);
	if (err_code != 0) {
		error_print(err_code);
		freeImage(&image);
		return err_code;
	}

	// Process image
	err_code = processImage(&image, argv[1]);
	if (err_code != 0) {
		error_print(err_code);
		freeImage(&image);
		return err_code;
	}

	// Write bmp from Image object
	err_code = writeBmp(argv[3], &image);
	if (err_code != 0) {
		error_print(err_code);
	}

	freeImage(&image);

	return 0;
}