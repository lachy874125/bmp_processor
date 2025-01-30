#ifndef IO_BMP_H
#define IO_BMP_H

#include "assert.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"

// Magic numbers
static const int BITS_IN_BYTE = 8;
static const size_t BMP_FILE_HEADER_SIZE = 14;
static const size_t BMP_INFO_HEADER_SIZE = 40;
static const int BMP_TOTAL_HEADER_SIZE = 54;

typedef struct InfoHeader {
	uint32_t size; // Size of this structure: must be 40
	int32_t width; // Image width
	int32_t height; // Image height; negative means top to bottom.
	uint32_t planes_bits; // Planes in 16 LSBs (must be 1); bits in 16 MSBs
	uint32_t compression; // Only accept 0 here (uncompressed RGB data)
	uint32_t image_size; // Can be 0
	int32_t xpels_per_metre; // Ignored
	int32_t ypels_per_metre; // Ignored
	uint32_t num_colours_used; // Entries in colour table (0 means use default)
	uint32_t num_colours_important; // 0 means all colours are important.
} InfoHeader;
/*  This header structure must be preceded by a 14 byte field, whose
	first 2 bytes contain the string, "BM", and whose next 4 bytes contain
	the length of the entire file.  The next 4 bytes must be 0. The final
	4 bytes provides an offset from the start of the file to the first byte
	of image sample data.
	If the bit_count is 1, 4 or 8, the structure must be followed by
	a colour lookup table, with 4 bytes per entry, the first 3 of which
	identify the blue, green and red intensities, respectively. */

typedef struct BmpIn {
	int num_components;
	int32_t rows;
	int32_t cols;
	int num_unread_rows;
	int line_bytes; // Number of bytes in each line, excluding padding
	int alignment_bytes; // Bytes at end of each line to make a multiple of 4.
	FILE* in;
} BmpIn;

int bmpInOpen(BmpIn* const bmp_in, const char* const fname);
/*  Opens the image file with the indicated name, initializing the supplied
	bmp_in structure to hold working state information for subsequent use
	with `bmpInClose()' and `bmpInGetLine()'.
	If an error occurs, the function returns one of the error codes
	`IO_ERR_NO_FILE', `IO_ERR_FILE_HEADER', `IO_ERR_FILE_TRUNC' or
	`IO_ERR_UNSUPPORTED'. Otherwise, the function returns 0 for success. */

void bmpInClose(BmpIn* const bmp_in);

int bmpInGetLine(BmpIn* const bmp_in, uint8_t* const line);
/*  Reads the next line of image data from the file opened using the most
	recent successful call to `bmpInOpen' (with the same bmp_in
	structure), storing the recovered samples in the supplied `line' buffer.
	ImageComps are interleaved in BGR order within the `line' buffer.
	If successful, the function returns 0.  If the file terminates
	unexpectedly, the `IO_ERR_FILE_TRUNC' error code is returned.  If the
	file is not currently open, or the end has been reached, the
	`IO_ERR_FILE_NOT_OPEN' error code is returned. */

typedef struct BmpOut {
	int num_components;
	int32_t rows;
	int32_t cols;
	int num_unwritten_rows;
	int line_bytes; // Number of bytes in each line, not including padding
	int alignment_bytes; // Number of 0's at end of each line.
	FILE* out;
} BmpOut;

int bmpOutOpen(BmpOut* const bmp_out, const char* const fname, const int width, const int height, const int num_components);
/*  Opens an image file with the indicated name for writing, initializing
	the supplied bmp_out structure to hold working state information for
	subsequent use with `bmpOutClose()' and `bmpOutWriteLine()'.
	The `num_components' value should be 1 for a monochrome image and 3
	for a colour image.
	The function returns 0 if successful, `IO_ERR_NO_FILE' if the file
	cannot be opened, or else `IO_ERR_SUPPORTED' if an illegal combination
	of parameters is supplied. */

void bmpOutClose(BmpOut* const bmp_out);

int bmpOutWriteLine(BmpOut* const bmp_out, const uint8_t* const line);
/*  Writes the next line of image data to the file opened using the most
	recent successful call to `bmpOutOpen' (with the same bmp_out
	structure), writing the samples supplied via the `line' buffer.
	ImageComps should be interleaved in BGR order within the `line' buffer.
	If successful, the function returns 0.  If the file cannot be written
	(e.g., the disk may be full), the `IO_ERR_FILE_TRUNC' error code is
	returned.  If the file is not currently open, or the end has been
	reached, the `IO_ERR_FILE_NOT_OPEN' error code is returned. */

#endif // IO_BMP_H
