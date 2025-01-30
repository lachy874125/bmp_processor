#include "process.h"
#include "string.h"
#include "error.h"
#include "stdio.h"
#include "stdlib.h"
#include "errno.h"


Error scaleRgb(Image* const image, uint8_t scale_red, uint8_t scale_green, uint8_t scale_blue) {
	if (image == NULL) return NULL_IMAGE;
	if (image->num_components != 3) return INVALID_IMAGE_RGB;

	if (scale_red < 100) {
		printf("Scaling red colour plane to %d%%\n", scale_red);
		Error err_code = scaleImageComp(image, colour_red, scale_red);
		if (err_code != SUCCESS) return err_code;
	}
	if (scale_green < 100) {
		printf("Scaling green colour plane to %d%%\n", scale_green);
		Error err_code = scaleImageComp(image, colour_green, scale_green);
		if (err_code != SUCCESS) return err_code;
	}
	if (scale_blue < 100) {
		printf("Scaling blue colour plane to %d%%\n", scale_blue);
		Error err_code = scaleImageComp(image, colour_blue, scale_blue);
		if (err_code != SUCCESS) return err_code;
	}

	return SUCCESS;
}


Error scaleImageComp(Image* const image, const Colour colour, uint8_t scale) {
	if (image == NULL) return NULL_IMAGE;
	if (image->components == NULL) return NULL_IMAGE_COMP;

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


Error initFilter(Filter** filter) {
	Filter* temp = (Filter*)calloc(1, sizeof(Filter));
	if (temp == NULL) return IO_ERR_ALLOC;

	*filter = temp;
	return SUCCESS;
}


void freeFilter(Filter* const filter) {
	if (filter == NULL) return;
	if (filter->data != NULL) free(filter->data);
	filter->data = NULL;
	filter->radius = 0;
}


Error parseFilter(Filter* const filter, const char* const filepath) {
	FILE* file = fopen(filepath, "r");
	if (!file) return IO_ERR_NO_FILE;

	char line[1024];
	int radius;

	// Read radius from first line
	if (!fgets(line, sizeof(line), file)) {
		fclose(file);
		return IO_ERR_FILE_TRUNC;
	}
	if (sscanf(line, "%d", &radius) != 1) {
		fclose(file);
		return INVALID_RADIUS_FORMAT;
	}
	if (radius < 0) {
		fclose(file);
		return NEGATIVE_RADIUS;
	}

	int diameter = 2 * radius + 1;

	filter->data = (float*)malloc(diameter * diameter * sizeof(float));
	if (filter->data == NULL) {
		fclose(file);
		return IO_ERR_ALLOC;
	}

	// Read filter data
	for (int i = 0; i < diameter; ++i) {
		// Read row
		if (!fgets(line, sizeof(line), file)) {
			free(filter->data);
			filter->data = NULL;
			fclose(file);
			return IO_ERR_FILE_TRUNC;
		}

		// Tokenise row
		char* token = strtok(line, ",\r\n");
		if (!token) {
			free(filter->data);
			filter->data = NULL;
			fclose(file);
			return INVALID_FILTER_FORMAT;
		}

		// Parse tokenised row
		for (int j = 0; j < diameter; ++j) {
			if (strlen(token) == 0) {
				free(filter->data);
				filter->data = NULL;
				fclose(file);
				return INVALID_FILTER_FORMAT;
			}

			// Parse token
			char* endptr;
			errno = 0;
			float value = strtof(token, &endptr);
			if (errno != 0 || *endptr != '\0') {
				free(filter->data);
				filter->data = NULL;
				fclose(file);
				return INVALID_FILTER_DATA;
			}
			filter->data[i * diameter + j] = value;

			// Get next token
			token = strtok(NULL, ",\r\n");
			if (j < diameter - 1 && !token) {
				free(filter->data);
				filter->data = NULL;
				fclose(file);
				return INVALID_FILTER_FORMAT;
			}
		}

		// Handle too many values
		if (strtok(NULL, ",") != NULL) {
			free(filter->data);
			filter->data = NULL;
			fclose(file);
			return INVALID_FILTER_FORMAT;
		}
	}

	filter->radius = radius;
	fclose(file);
	return SUCCESS;
}


Error applyFilterComp(ImageComp* const image_comp, const Filter* const filter) {
	if (image_comp == NULL) return NULL_IMAGE_COMP;
	if (filter == NULL) return NULL_FILTER;

	int radius = filter->radius;
	int diameter = 2 * radius + 1;

	const int width = image_comp->width;
	const int x_border = image_comp->x_border;
	const int total_width = width + 2 * x_border;
	const int height = image_comp->height;
	const int y_border = image_comp->y_border;
	const int total_height = height + 2 * y_border;

	// Copy image component
	ImageComp copy;
	copy.width = width;
	copy.height = height;
	copy.x_border = x_border;
	copy.y_border = y_border;
	copy.data = (uint8_t*)malloc(total_width * total_height * sizeof(uint8_t));
	if (copy.data == NULL) return IO_ERR_ALLOC;
	copy.image = copy.data + y_border * total_width + x_border;

	for (int r = 0; r < total_height; ++r) {
		for (int c = 0; c < total_width; ++c) {
			copy.data[r * total_width + c] = image_comp->data[r * total_width + c];
		}
	}

	// Perform convolution
	float* filter_centre = filter->data + radius * diameter + radius;
	for (int r = 0; r < height; ++r) {
		for (int c = 0; c < width; ++c) {
			uint8_t* src = copy.image + r * total_width + c;
			uint8_t* dst = image_comp->image + r * total_width + c;
			float sum = 0;
			for (int y = -radius; y <= radius; ++y) {
				for (int x = -radius; x <= radius; ++x) {
					sum += (float)src[y * total_width + x] * filter_centre[-y * diameter - x];
				}
			}
			*dst = (uint8_t)sum;
		}
	}

	free(copy.data);

	return SUCCESS;
}


Error applyFilter(Image* const image, const Filter* const filter) {
	if (image == NULL) return NULL_IMAGE;
	if (image->components == NULL) return NULL_IMAGE_COMP;
	if (filter == NULL) return NULL_FILTER;

	for (int p = 0; p < image->num_components; ++p) {
		Error err_code = applyFilterComp(image->components + p, filter);
		if (err_code != SUCCESS) return err_code;
	}

	return SUCCESS;
}


Error copyImage(const Image* const image, Image** const copy) {
	if (*copy != NULL) freeImage(*copy);

	Error err_code = initImage(copy);
	if (err_code != SUCCESS) return err_code;

	(*copy)->num_components = image->num_components;
	(*copy)->components = (ImageComp*)malloc(image->num_components * sizeof(ImageComp));
	if ((*copy)->components == NULL) {
		err_code = IO_ERR_ALLOC;
		freeImage(*copy);
		return err_code;
	}

	for (int p = 0; p < image->num_components; ++p) {
		ImageComp* const source = image->components + p;
		ImageComp* const dest = (*copy)->components + p;

		dest->width = source->width;
		dest->height = source->height;
		dest->x_border = source->x_border;
		dest->y_border = source->y_border;

		dest->data = (uint8_t*)malloc(dest->width * dest->height * sizeof(uint8_t));
		if (dest->data == NULL) {
			err_code = IO_ERR_ALLOC;

			// Ensure only allocated components are freed in freeImage()
			(*copy)->num_components = p;

			freeImage(*copy);
			return err_code;
		}

		memcpy(dest->data, source->data, source->width * source->height * sizeof(uint8_t));

		dest->image = dest->data + dest->y_border * dest->width + dest->x_border;
	}

	return SUCCESS;
}