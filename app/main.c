#include "io_bmp.h"
#include "image.h"
#include "error.h"

int main(int argc, char* argv[]) {
	// Handle invalid arguments
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <bmp_in bmp file> <bmp_out bmp file>\n", argv[0]);
		return -1;
	}

	// Read bmp into Image object
	Image image;
	int err_code = readBmp(argv[1], &image);
	if (err_code != 0) {
		error_print(err_code);
		freeImage(&image);
		return err_code;
	}

	// Write bmp from Image object
	err_code = writeBmp(argv[2], &image);
	if (err_code != 0) {
		error_print(err_code);
	}

	freeImage(&image);

	return 0;
}