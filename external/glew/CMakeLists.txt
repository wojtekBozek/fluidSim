cmake_minimum_required(VERSION 3.14)
# Fetch and include glew
include(FetchContent)
FetchContent_Declare(
    glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG        glew-cmake-2.2.0
)
FetchContent_MakeAvailable(glew)