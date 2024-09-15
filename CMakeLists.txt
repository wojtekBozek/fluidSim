cmake_minimum_required(VERSION 3.14)

project(FluidSimulator)

add_subdirectory(external)

file(GLOB_RECURSE _main CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_executable(${PROJECT_NAME} ${_main})
add_definitions(-DGLEW_STATIC)
target_link_libraries(${PROJECT_NAME} opengl32 glfw libglew_static glm)
