# BMP Image Processor

This project is a recreational and learning endeavour. Currently, it is simply a library for reading and writing .bmp files. It is compatibly with most common .bmp file formats. I intend for it to become a command-line tool to perform various image processing actions. The implementation will remain entirely in the C programming language.

## Features

- Reads and writes 8-bit (grayscale) and 24-bit (color) BMP files
- Handles both little and big-endian architectures
- No external dependencies beyond the C standard library

## Build

```bash
gcc  main.c io_bmp.c -o bmp_processor
```

## Usage
```bash
./bmp_processor input.bmp output.bmp
```

## Roadmap
- Basic geometric transformations (scaling, rotation)
- Image filtering