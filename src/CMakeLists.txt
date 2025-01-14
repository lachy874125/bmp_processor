add_library(
	bmp_lib
	io_bmp.c
	image.c
)

target_include_directories(bmp_lib PUBLIC ${PROJECT_SOURCE_DIR}/include/bmp_processor)