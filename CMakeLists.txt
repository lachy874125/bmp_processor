cmake_minimum_required(VERSION 3.28)

project(bmp_processor
	VERSION 1.0
	DESCRIPTION "BMP image processor"
	LANGUAGES C
)

add_subdirectory(src)
add_subdirectory(app)