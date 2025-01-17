#ifndef ERROR_H
#define ERROR_H

// Error codes
typedef enum {
    SUCCESS = 0,					// No error
    IO_ERR_NO_FILE = -1,			// File not found
	IO_ERR_FILE_HEADER = -2,		// Header has an error
	IO_ERR_FILE_TRUNC = -3,			// File ends unexpectely
	IO_ERR_UNSUPPORTED = -4,		// Exception code if file uses an unsupported format
	IO_ERR_FILE_NOT_OPEN = -5,		// Trying to read/write a file which is not open, or has come to the end
	IO_ERR_ALLOC = -6,				// Memory allocation fails
	ISOLATE_GREYSCALE = -7,			// User attempts to isolate colour component from greyscale image
	INVALID_COMMAND = -8,			// Invalid command
	INVALID_COMMAND_ISOLATE = -9,	// Isolate colour/component command is invalid
	NULL_IMAGE = -10,				// Image is null
	NULL_COMPONENT = -11			// Component is null
} error_t;

// Error printing functions
const char* error_string(error_t error);
void error_print(error_t error);

#endif	// ERROR_H