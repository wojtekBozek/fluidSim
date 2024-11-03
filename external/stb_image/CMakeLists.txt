# external/stb_image/CMakeLists.txt
cmake_minimum_required(VERSION 3.14)

# Create an interface library for stb_image
add_library(stb_image INTERFACE)

# Include the directory containing stb_image.h
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})