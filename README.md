# BMP Image Processor

This project is a recreational and learning endeavour in the C programming language. Currently, it is simply a library for reading and writing BMP image files. It is compatible with most common BMP file formats. I intend for it to become a command-line tool to perform various image processing actions.

## Features

- Reads and writes 8-bit (grayscale) and 24-bit (color) BMP files
- Isolates any combination of the red, green, and blue colour planes
- Handles both little and big-endian architectures
- No external dependencies beyond the C standard library

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Usage

Currently, the tool allows you to scale pixel values of color planes from RGB BMP images to between 0% and 100%.

### Basic usage:
```bash
./build/app/bmp_processor scale-rgb:<colour_options> <input_file> <output_file>
```

where:
- `<colour_options>`: Any combination of `r=<scale>`, `g=<scale>`, `b=<scale>` separated by commas `,`. Order is irrelevant. If any `colour=<scale>` pattern is absent, that colour is not scaled (remains at 100%).
- `<input_file>`: Path to the input BMP file.
- `<output_file>`: Path where the processed BMP file will be saved.

### Examples:
```bash
# Scale each plane by some amount
./build/app/bmp_processor scale-rgb:r=50,g=34,b=77 <input_file> <output_file>

# Remove the red and blue planes (isolate the green plane)
./build/app/bmp_processor scale-rgb:r=0,b=0 <input_file> <output_file>

# Remove the green plane
./build/app/bmp_processor scale-rgb:g=0 <input_file> <output_file>

# Keep all planes (no change)
./build/app/bmp_processor scale-rgb:r=100,g=100,b=100 <input_file> <output_file>
```

## Roadmap
- Basic geometric transformations (scaling, rotation)
- Image filtering