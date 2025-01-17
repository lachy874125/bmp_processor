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

Currently, the tool allows you to isolate specific color planes from RGB BMP images.

### Basic usage:
```bash
./build/app/bmp_processor isolate-<colour_options> <input_file> <output_file>
```

where:
- `<colour_options>`: Any combination of `r` (red), `g` (green), and `b` (blue) characters specifying which color planes to keep. For example, `rb` keeps red and blue planes while removing green. The order of the characters is irrelevant.

- `<input_file>`: Path to the input BMP file
- `<output_file>`: Path where the processed BMP file will be saved

### Examples:
```bash
# Keep only the green plane
./build/app/bmp_processor isolate-g <input_file> <output_file>

# Keep only the red and blue color planes
./build/app/bmp_processor isolate-rb <input_file> <output_file>

# Keep all planes (no change)
./build/app/bmp_processor isolate-rgb <input_file> <output_file>
```

## Roadmap
- Basic geometric transformations (scaling, rotation)
- Image filtering