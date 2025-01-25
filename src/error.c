#include "stdio.h"
#include "error.h"

const char* error_string(error_t error) {
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
        case NULL_COMPONENT:
            return "Component is null.";
        case INVALID_SCALE:
            return "Invalid scaling factor.";
        default:
            return "Unknown error";
    }
}

void error_print(error_t error) {
    fprintf(stderr, "Error: %s\n", error_string(error));
}