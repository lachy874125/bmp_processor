#include "stdio.h"
#include "error.h"

const char* getErrorString(Error error) {
    switch (error) {
        case SUCCESS:
            return "Success";
        case IO_ERR_NO_FILE:
            return "Cannot open supplied input or output file.";
        case IO_ERR_UNSUPPORTED:
            return "Input uses an unsupported BMP file format. Current simple example supports only 8-bit and 24-bit data.";
        case IO_ERR_FILE_TRUNC:
            return "Input or output file truncated unexpectedly.";
        case IO_ERR_FILE_NOT_OPEN:
            return "Trying to access a file which is not open!(?)";
        case IO_ERR_ALLOC:
            return "Memory allocation failed.";
        case INVALID_IMAGE_RGB:
            return "Image is not in RGB format.";
        case INVALID_COMMAND:
            return "Invalid command.";
        case NULL_IMAGE:
            return "Image is null.";
        case NULL_IMAGE_COMP:
            return "ImageComp is null.";
        case NULL_FILTER:
            return "Filter is null.";
        case NULL_FILTER_DATA:
            return "Filter data is null.";
        case INVALID_FILTER_FORMAT:
            return "Invalid filter format.";
        case INVALID_FILTER_DATA:
            return "Invalid filter data.";
        case INVALID_SCALE:
            return "Invalid scaling factor.";
        case INSUFFICIENT_BORDER:
            return "Insufficient border size.";
        case INVALID_RADIUS_FORMAT:
            return "Invalid radius format.";
        case NEGATIVE_RADIUS:
            return "Radius must be non-negative.";
        case BORDER_TOO_LARGE:
            return "Border must be less than or equal to image dimensions.";
        default:
            return "Unknown error";
    }
}

void printErrorString(Error error) {
    fprintf(stderr, "Error: %s\n", getErrorString(error));
}