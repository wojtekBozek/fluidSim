cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(
  imgui
  URL https://github.com/ocornut/imgui/archive/docking.zip
  SOURCE_DIR ${PARENT_DIRECTORY}/imgui_glfw/imgui
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(imgui)