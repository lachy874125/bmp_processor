# BMP Image Processor

This project is a work-in-progress recreational and learning endeavour. Currently, it is simply a library for reading, processing, and writing BMP image files with a command-line tool to interact with the library. It is compatible with most common BMP file formats. The library reads BMP files into an Image UDT that allows for easier implementation of image processing tasks. It can also write a new BMP file from an Image UDT. The currently-implemented image processing functions are:

- [Scale RGB](#scale-rgb)

## Build

```bash
cmake -S . -B build
cmake --build build
```

### Scale RGB
Allows you to scale pixel values of color planes from RGB BMP images to between 0% and 100%.
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