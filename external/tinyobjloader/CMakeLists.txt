cmake_minimum_required(VERSION 3.14)
# Fetch and include tinyObjLoader
include(FetchContent)
FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG        v1.0.6
)
FetchContent_MakeAvailable(tinyobjloader)