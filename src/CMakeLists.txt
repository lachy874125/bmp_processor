add_library(
	bmp_lib
	io_bmp.c
	image.c
	error.c
	process.c
)

target_compile_options(bmp_lib PRIVATE -Wall -Wextra)


target_include_directories(bmp_lib PUBLIC ${PROJECT_SOURCE_DIR}/include/bmp_processor)