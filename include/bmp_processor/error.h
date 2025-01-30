#ifndef ERROR_H
#define ERROR_H

// Error codes
typedef enum {
    SUCCESS,					// No error
    IO_ERR_NO_FILE,				// File not found
	IO_ERR_FILE_HEADER,			// Header has an error
	IO_ERR_FILE_TRUNC,			// File ends unexpectedly
	IO_ERR_UNSUPPORTED,			// Exception code if file uses an unsupported format
	IO_ERR_FILE_NOT_OPEN,		// Trying to read/write a file which is not open, or has come to the end
	IO_ERR_ALLOC,				// Memory allocation fails
	INVALID_IMAGE_RGB,			// Image is not in RGB format
	INVALID_COMMAND,			// Invalid command
	NULL_IMAGE,					// Image is null
	NULL_IMAGE_COMP,			// ImageComp is null
	NULL_FILTER,				// Filter is null
	NULL_FILTER_DATA,			// Filter data is null
	INVALID_FILTER_FORMAT,		// Invalid filter format
	INVALID_FILTER_DATA,		// Invalid filter data
	INVALID_SCALE,				// Invalid scaling factor
	INSUFFICIENT_BORDER,		// Insufficient border size
	INVALID_RADIUS_FORMAT,		// Invalid radius format
	NEGATIVE_RADIUS,			// Negative radius
	BORDER_TOO_LARGE,			// Border too large
} Error;

// Error printing functions
const char* getErrorString(Error error);
void printErrorString(Error error);

#endif	// ERROR_H