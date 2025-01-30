#include "string.h"
#include "io_bmp.h"
#include "error.h"

static void toLittleEndian(int32_t* words, int num_words) {
	const int32_t test = 1; // 4-byte value
	const uint8_t* first_byte = (uint8_t*)&test; // Read only the first byte
	if (*first_byte) return; // Machine uses little-endian architecture already

	// Convert to little-endian
	while (num_words > 0) {
		int32_t tmp = *words;
		*words = ((tmp >> 24) & 0x000000FF) +
			((tmp >> 8) & 0x0000FF00) +
			((tmp << 8) & 0x00FF0000) +
			((tmp << 24) & 0xFF000000);
		++words;
	}
}

int bmpInOpen(BmpIn* const bmp_in, const char* const fname) {
	// Reset everything
	memset(bmp_in, 0, sizeof(BmpIn));

	// Open file in read-binary mode
	bmp_in->in = fopen(fname, "rb");
	if (bmp_in->in == NULL) return(IO_ERR_NO_FILE);

	// Read file info_header
	uint8_t file_header[BMP_FILE_HEADER_SIZE];
	if (fread(file_header, 1, BMP_FILE_HEADER_SIZE, bmp_in->in) != BMP_FILE_HEADER_SIZE) return(IO_ERR_FILE_TRUNC);

	// Check file info_header
	if ((file_header[0] != 'B') || (file_header[1] != 'M')) return(IO_ERR_FILE_HEADER);

	// Read info info_header
	InfoHeader info_header;
	if (fread(&info_header, 1, BMP_INFO_HEADER_SIZE, bmp_in->in) != BMP_INFO_HEADER_SIZE) return(IO_ERR_FILE_TRUNC);

	toLittleEndian((int32_t*)&info_header, 10);
	bmp_in->cols = info_header.width;
	bmp_in->rows = info_header.height;
	int bit_count = (info_header.planes_bits >> 16);
	if (bit_count == 24) bmp_in->num_components = 3;
	else if (bit_count == 8) bmp_in->num_components = 1;
	else return(IO_ERR_UNSUPPORTED);

	// Calculate total info_header size
	int palette_entries_used = info_header.num_colours_used;
	if (bmp_in->num_components != 1) palette_entries_used = 0;
	else if (info_header.num_colours_used == 0) palette_entries_used = (1 << bit_count);
	const int header_size = BMP_TOTAL_HEADER_SIZE + 4 * palette_entries_used;

	// Read little-endian offset
	int offset = file_header[13];
	offset <<= BITS_IN_BYTE; offset += file_header[12];
	offset <<= BITS_IN_BYTE; offset += file_header[11];
	offset <<= BITS_IN_BYTE; offset += file_header[10];
	if (offset < header_size) return(IO_ERR_FILE_HEADER);
	if (palette_entries_used) fseek(bmp_in->in, palette_entries_used * 4, SEEK_CUR); // Skip over palette
	if (offset > header_size) fseek(bmp_in->in, offset - header_size, SEEK_CUR);
	bmp_in->num_unread_rows = bmp_in->rows;
	bmp_in->line_bytes = bmp_in->num_components * bmp_in->cols;
	bmp_in->alignment_bytes = (4 - bmp_in->line_bytes) & 3; // Pad to a multiple of 4 bytes
	return SUCCESS;
}


void bmpInClose(BmpIn* const bmp_in) {
	if (bmp_in->in != NULL) fclose(bmp_in->in);
	memset(bmp_in, 0, sizeof(BmpIn));
}

int bmpInGetLine(BmpIn* const bmp_in, uint8_t* const line) {
	// Read next line
	if ((bmp_in->in == NULL) || (line == NULL) || (bmp_in->num_unread_rows <= 0)) return(IO_ERR_FILE_NOT_OPEN);
	bmp_in->num_unread_rows--;
	if (fread(line, 1, (size_t)bmp_in->line_bytes, bmp_in->in) != (size_t)bmp_in->line_bytes) return(IO_ERR_FILE_TRUNC);

	// Read padding
	if (bmp_in->alignment_bytes > 0) {
		uint8_t buf[3];
		if (fread(buf, 1, (size_t)bmp_in->alignment_bytes, bmp_in->in) != (size_t)bmp_in->alignment_bytes) return(IO_ERR_FILE_TRUNC);
	}
	return SUCCESS;
}

int bmpOutOpen(BmpOut* const bmp_out, const char* const fname, const int width, const int height, const int num_components) {

	// Reset everything
	memset(bmp_out, 0, sizeof(BmpOut));

	bmp_out->num_components = num_components;
	bmp_out->rows = bmp_out->num_unwritten_rows = height;
	bmp_out->cols = width;
	uint8_t file_header[BMP_FILE_HEADER_SIZE];
	InfoHeader info_header;
	int header_bytes = BMP_FILE_HEADER_SIZE + sizeof(info_header);
	assert(header_bytes == BMP_TOTAL_HEADER_SIZE);
	if (num_components == 1) header_bytes += 1024; // Include colour lookup table
	else if (num_components != 3) return(IO_ERR_UNSUPPORTED);

	bmp_out->line_bytes = num_components * width;
	bmp_out->alignment_bytes = (4 - bmp_out->line_bytes) & 3;

	// Prepare file header
	const int file_bytes = header_bytes + (bmp_out->line_bytes + bmp_out->alignment_bytes) * bmp_out->rows;
	file_header[0] = 'B'; file_header[1] = 'M';
	file_header[2] = (uint8_t)file_bytes;
	file_header[3] = (uint8_t)(file_bytes >> 8);
	file_header[4] = (uint8_t)(file_bytes >> 16);
	file_header[5] = (uint8_t)(file_bytes >> 24);
	file_header[6] = file_header[7] = file_header[8] = file_header[9] = 0;
	file_header[10] = (uint8_t)header_bytes;
	file_header[11] = (uint8_t)(header_bytes >> 8);
	file_header[12] = (uint8_t)(header_bytes >> 16);
	file_header[13] = (uint8_t)(header_bytes >> 24);
	info_header.size = BMP_INFO_HEADER_SIZE;
	info_header.width = width;
	info_header.height = height;
	info_header.planes_bits = 1; // Set `planes'=1 (mandatory)
	info_header.planes_bits |= ((num_components == 1) ? 8 : 24) << 16; // Set bits per pel.
	info_header.compression = 0;
	info_header.image_size = 0;
	info_header.xpels_per_metre = info_header.ypels_per_metre = 0;
	info_header.num_colours_used = info_header.num_colours_important = 0;
	toLittleEndian((int32_t*)&info_header, 10);

	// Open file in write-binary mode
	bmp_out->out = fopen(fname, "wb");
	if (bmp_out->out == NULL) return(IO_ERR_NO_FILE);

	// Write header
	fwrite(file_header, 1, BMP_FILE_HEADER_SIZE, bmp_out->out);
	fwrite(&info_header, 1, BMP_INFO_HEADER_SIZE, bmp_out->out);

	// Write grey-scale palette
	if (num_components == 1) {
		for (int n = 0; n < 256; n++) {
			fputc(n, bmp_out->out);
			fputc(n, bmp_out->out);
			fputc(n, bmp_out->out);
			fputc(0, bmp_out->out);
		}
	}
	return SUCCESS;
}

void bmpOutClose(BmpOut* const bmp_out) {
	if (bmp_out->out != NULL) fclose(bmp_out->out);
	memset(bmp_out, 0, sizeof(BmpOut));
}

int bmpOutWriteLine(BmpOut* const bmp_out, const uint8_t* const line) {
	// Write next line
	if ((bmp_out->out == NULL) || (line == NULL) || (bmp_out->num_unwritten_rows <= 0)) return(IO_ERR_FILE_NOT_OPEN);
	bmp_out->num_unwritten_rows--;
	if (fwrite(line, 1, (size_t)bmp_out->line_bytes, bmp_out->out) != (size_t)bmp_out->line_bytes) return IO_ERR_FILE_TRUNC;

	// Write padding
	if (bmp_out->alignment_bytes > 0) {
		const uint8_t buf[3] = { 0,0,0 };
		fwrite(buf, 1, (size_t)bmp_out->alignment_bytes, bmp_out->out);
	}
	return SUCCESS;
}