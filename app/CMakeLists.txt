add_executable(bmp_processor main.c)

target_compile_options(bmp_processor PRIVATE -Wall -Wextra)

target_link_libraries(bmp_processor PRIVATE bmp_lib)