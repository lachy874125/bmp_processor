#ifndef ERROR_H
#define ERROR_H

// Error codes
typedef enum {
    SUCCESS,					// No error
    IO_ERR_NO_FILE,				// File not found
	IO_ERR_FILE_HEADER,			// Header has an error
	IO_ERR_FILE_TRUNC,			// File ends unexpectely
	IO_ERR_UNSUPPORTED,			// Exception code if file uses an unsupported format
	IO_ERR_FILE_NOT_OPEN,		// Trying to read/write a file which is not open, or has come to the end
	IO_ERR_ALLOC,				// Memory allocation fails
	INVALID_IMAGE_RGB,			// Image is not in RGB format
	INVALID_COMMAND,			// Invalid command
	NULL_IMAGE,					// Image is null
	NULL_COMPONENT,				// Component is null
	INVALID_SCALE				// Invalid scaling factor
} error_t;

// Error printing functions
const char* error_string(error_t error);
void error_print(error_t error);

#endif	// ERROR_H