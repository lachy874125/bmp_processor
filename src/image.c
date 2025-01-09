#include "io_bmp.h"
#include "image.h"

void freeImage(Image* image) {
	if (image == NULL) return;

	if (image->components != NULL) {
		for (int c = 0; c < image->num_components; ++c) {
			free(image->components[c].data);
		}
		free(image->components);
	}
	image->components = NULL;
	image->num_components = 0;
}

int readBmp(const char* in_file, Image* image) {
	int err_code;

	// Read the input image header
	BmpIn bmp_in;
	err_code = bmpInOpen(&bmp_in, in_file);
	if (err_code != 0) return err_code;

	// Allocate memory
	const int width = bmp_in.cols;
	const int height = bmp_in.rows;
	const int num_components = bmp_in.num_components;
	uint8_t* data = (uint8_t*)malloc(num_components * width * height * sizeof(uint8_t));
	if (data == NULL) {
		bmpInClose(&bmp_in);
		err_code = IO_ERR_ALLOC;
		return err_code;
	}

	// Read the input image data into array
	int n = height;
	uint8_t* line = data;
	while (n > 0) {
		err_code = bmpInGetLine(&bmp_in, line);
		if (err_code != 0) {
			bmpInClose(&bmp_in);
			free(data);
			return err_code;
		}
		--n;
		line += width * num_components;
	}

	// Close the input image
	bmpInClose(&bmp_in);

	// Initialise colour-component-separated Image object
	image->num_components = num_components;
	image->components = (Component*)malloc(num_components * sizeof(Component));
	if (image->components == NULL) {
		err_code = IO_ERR_ALLOC;
		free(data);
		return err_code;
	}

	// Populate image object
	for (size_t c = 0; c < num_components; ++c) {
		Component* component = image->components + c;
		component->width = width;
		component->height = height;

		// Copy data to each component
		component->data = (uint8_t*)malloc(width * height * sizeof(uint8_t));
		if (component->data == NULL) {
			err_code = IO_ERR_ALLOC;

			// Ensure only allocated components are freed by main
			image->num_components = c;

			free(data);
			return err_code;
		}
		for (size_t j = 0; j < width * height; ++j) {
			component->data[j] = data[num_components * j + c];
		}
	}

	free(data);

	return 0;
}

int writeBmp(char* out_file, const Image* image) {
	int err_code;

	// Retrieve image component properties
	int width = image->components[0].width;
	int height = image->components[0].height;
	int planes = image->num_components;

	uint8_t* data = (uint8_t*)malloc(planes * width * height * sizeof(uint8_t));
	if (data == NULL) {
		err_code = IO_ERR_ALLOC;
		return err_code;
	}

	// Copy from plane-separated image object to interleaved BGR array
	for (int p = 0; p < planes; ++p) {
		for (size_t j = 0; j < width * height; ++j) {
			data[planes * j + p] = image->components[p].data[j];
		}
	}

	// // Alternate copying method but slower due to division
	// for (size_t j = 0; j < width * planes * height; ++j) {
	// 	data[j] = image->components[j % planes].data[j / planes];
	// }


	// Write the image to the output .bmp file
	BmpOut bmp_out;
	err_code = bmpOutOpen(&bmp_out, out_file, width, height, planes);
	if (err_code != 0) {
		free(data);
		return err_code;
	}

	int n = height;
	uint8_t* line = data;
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