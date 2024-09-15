cmake_minimum_required(VERSION 3.14)
# Fetch and include imgui
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.90.2
)
FetchContent_MakeAvailable(imgui)