#include "io_bmp.h"
#include "image.h"
#include "error.h"


Error initImage(Image** image) {
	Image* temp = (Image*)calloc(1, sizeof(Image));
	if (temp == NULL) return IO_ERR_ALLOC;

	*image = temp;
	return SUCCESS;
}


void freeImage(Image* const image) {
	if (image == NULL) return;

	if (image->components != NULL) {
		for (int p = 0; p < image->num_components; ++p) {
			free(image->components[p].data);
		}
		free(image->components);
	}
	image->components = NULL;
	image->num_components = 0;
}


Error readBmp(Image* const image, const char* const in_file, int x_border, int y_border) {
	int err_code;

	// Read the input image header
	BmpIn bmp_in;
	err_code = bmpInOpen(&bmp_in, in_file);
	if (err_code != SUCCESS) return err_code;

	// Allocate memory for a row of pixel data
	const int width = bmp_in.cols;
	const int height = bmp_in.rows;
	const int total_width = width + 2 * x_border;
	const int total_height = height + 2 * y_border;
	const int num_components = bmp_in.num_components;
	uint8_t* const line = (uint8_t*)malloc(num_components * width * sizeof(uint8_t));
	if (line == NULL) {
		bmpInClose(&bmp_in);
		err_code = IO_ERR_ALLOC;
		return err_code;
	}

	// Allocate memory for Image components
	image->num_components = num_components;
	image->components = (ImageComp*)malloc(num_components * sizeof(ImageComp));
	if (image->components == NULL) {
		err_code = IO_ERR_ALLOC;
		bmpInClose(&bmp_in);
		free(line);
		return err_code;
	}

	// Allocate memory for Image data
	for (int p = 0; p < num_components; ++p) {
		ImageComp* component = image->components + p;
		component->width = width;
		component->height = height;
		component->x_border = x_border;
		component->y_border = y_border;

		component->data = (uint8_t*)malloc(total_width * total_height * sizeof(uint8_t));
		if (component->data == NULL) {
			err_code = IO_ERR_ALLOC;

			// Ensure only allocated components are freed in freeImage()
			image->num_components = p;

			bmpInClose(&bmp_in);
			free(line);
			return err_code;
		}
		component->image = component->data + y_border * total_width + x_border;
	}

	// Copy BMP pixel data into colour components of Image object
	for (int r = 0; r < height; ++r) {
		// Read the input image data into array
		err_code = bmpInGetLine(&bmp_in, line);
		if (err_code != SUCCESS) {
			bmpInClose(&bmp_in);
			free(line);
			return err_code;
		}

		// Read data from array into colour components
		for (int p = 0; p < num_components; ++p) {
			uint8_t* src = line + p;
			uint8_t* const dst = image->components[p].image + r * total_width;

			for (int c = 0; c < width; ++c) {
				dst[c] = *src;
				src += num_components;
			}
		}
	}

	// Perform boundary extension
	err_code = extendBoundary(image);
	if (err_code != SUCCESS) {
		bmpInClose(&bmp_in);
		free(line);
		return err_code;
	}

	// Close the input image
	bmpInClose(&bmp_in);

	free(line);

	return SUCCESS;
}


Error extendBoundary(Image* const image) {
	if (image == NULL) return NULL_IMAGE;

	const int num_components = image->num_components;
	const int width = image->components[0].width;
	const int x_border = image->components[0].x_border;
	const int total_width = width + 2 * x_border;
	const int height = image->components[0].height;
	const int y_border = image->components[0].y_border;

	if (x_border > width || y_border > height) return BORDER_TOO_LARGE;

	for (int p = 0; p < num_components; ++p) {
		ImageComp* component = image->components + p;
		if (component == NULL) return NULL_IMAGE_COMP;

		// Extend horizontally
		for (int r = 0; r < height; ++r) {
			uint8_t* left_edge = image->components[p].image + r * total_width - 1;
			uint8_t* right_edge = left_edge + width + 1;
			for (int c = 0; c < x_border; ++c) {
				left_edge[-c] = left_edge[c + 1];
				right_edge[c] = right_edge[-1 - c];
			}
		}

		// Extend vertically
		for (int c = 0; c < total_width; ++c) {
			uint8_t* bottom_edge = image->components[p].data + (y_border - 1) * total_width + c;
			uint8_t* top_edge = bottom_edge + (height + 1) * total_width;
			for (int r = 0; r < y_border; ++r) {
				bottom_edge[-r * total_width] = bottom_edge[(r + 1) * total_width];
				top_edge[r * total_width] = top_edge[(-1 - r) * total_width];
			}
		}
	}

	return SUCCESS;
}


Error writeBmp(const Image* const image, const char* const out_file) {
	int err_code;

	// Retrieve image component properties
	const int width = image->components[0].width;
	const int height = image->components[0].height;
	const int total_width = width + 2 * image->components[0].x_border;
	const int num_components = image->num_components;

	BmpOut bmp_out;
	err_code = bmpOutOpen(&bmp_out, out_file, width, height, num_components);
	if (err_code != SUCCESS) return err_code;

	uint8_t* const line = (uint8_t*)malloc(num_components * width * sizeof(uint8_t));
	if (line == NULL) {
		err_code = IO_ERR_ALLOC;
		return err_code;
	}

	for (int r = 0; r < height; ++r) {
		// Copy from plane-separated image object to interleaved BGR array
		for (int p = 0; p < num_components; ++p) {
			uint8_t* const src = image->components[p].image + r * total_width;
			uint8_t* dst = line + p;

			for (int c = 0; c < width; ++c) {
				*dst = src[c];
				dst += num_components;
			}
		}

		// Write data from array into output image
		err_code = bmpOutWriteLine(&bmp_out, line);
		if (err_code != SUCCESS) {
			free(line);
			bmpOutClose(&bmp_out);
			return err_code;
		}
	}

	bmpOutClose(&bmp_out);
	free(line);

	return SUCCESS;
}