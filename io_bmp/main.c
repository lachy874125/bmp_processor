#include "io_bmp.h"

// Simple example of reading BMP file into a data array, creating a new BMP file and writing the data back out
int readWriteBmp(char* in_file, char* out_file) {
	int err_code;

	// Read the input image header
	BmpIn bmp_in;
	err_code = bmpInOpen(&bmp_in, in_file);
	if (err_code != 0) return err_code;

	// Allocate memory
	const int width = bmp_in.cols;
	const int height = bmp_in.rows;
	const int planes = bmp_in.num_components;
	uint8_t* data = (uint8_t*)malloc(width * height * planes * sizeof(uint8_t));
	if (data == NULL) {
		bmpInClose(&bmp_in);
		err_code = IO_ERR_ALLOC;
		return err_code;
	}

	// Read the input image data
	int n = height;
	uint8_t* line = data;
	while (n > 0) {
		err_code = bmpInGetLine(&bmp_in, line);
		if (err_code != 0) return err_code;

		--n;
		line += width * planes;
	}

	// Close the input image
	bmpInClose(&bmp_in);

	// Write the image back out again
	BmpOut bmp_out;
	err_code = bmpOutOpen(&bmp_out, out_file, width, height, planes);
	if (err_code != 0) return err_code;

	n = height;
	line = data;
	while (n > 0) {
		err_code = bmpOutWriteLine(&bmp_out, line);

		// Handle write errors
		if (err_code != 0) {
			free(data);
			bmpOutClose(&bmp_out);
			return err_code;
		}

		--n;
		line += width * planes;
	}

	bmpOutClose(&bmp_out);
	free(data);

	return 0;
}

int main(int argc, char* argv[]) {
	// Handle invalid arguments
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <bmp_in bmp file> <bmp_out bmp file>\n", argv[0]);
		return -1;
	}

	// Process the image
	int return_code = readWriteBmp(argv[1], argv[2]);

	// Handle error codes
	if (return_code == IO_ERR_NO_FILE) fprintf(stderr, "Cannot open supplied input or output file.\n");
	else if (return_code == IO_ERR_FILE_HEADER) fprintf(stderr, "Error encountered while parsing BMP file header.\n");
	else if (return_code == IO_ERR_UNSUPPORTED) fprintf(stderr, "Input uses an unsupported BMP file format.\n  Current simple example supports only 8-bit and 24-bit data.\n");
	else if (return_code == IO_ERR_FILE_TRUNC) fprintf(stderr, "Input or output file truncated unexpectedly.\n");
	else if (return_code == IO_ERR_FILE_NOT_OPEN) fprintf(stderr, "Trying to access a file which is not open!(?)\n");
	else if (return_code == IO_ERR_ALLOC) fprintf(stderr, "Memory allocation failed.\n");

	return 0;
}