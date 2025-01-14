#include "io_bmp.h"
#include "image.h"

void error(int err_code) {
	if (err_code == IO_ERR_NO_FILE) fprintf(stderr, "Cannot open supplied input or output file.\n");
	else if (err_code == IO_ERR_FILE_HEADER) fprintf(stderr, "Error encountered while parsing BMP file header.\n");
	else if (err_code == IO_ERR_UNSUPPORTED) fprintf(stderr, "Input uses an unsupported BMP file format.\n  Current simple example supports only 8-bit and 24-bit data.\n");
	else if (err_code == IO_ERR_FILE_TRUNC) fprintf(stderr, "Input or output file truncated unexpectedly.\n");
	else if (err_code == IO_ERR_FILE_NOT_OPEN) fprintf(stderr, "Trying to access a file which is not open!(?)\n");
	else if (err_code == IO_ERR_ALLOC) fprintf(stderr, "Memory allocation failed.\n");
}

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
		error(err_code);
		freeImage(&image);
		return err_code;
	}

	// Write bmp from Image object
	err_code = writeBmp(argv[2], &image);
	if (err_code != 0) {
		error(err_code);
	}

	freeImage(&image);

	return 0;
}