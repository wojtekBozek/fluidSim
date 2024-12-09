cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(
  imgui
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SOURCE_DIR ${PARENT_DIRECTORY}/imgui_glfw/imgui
  URL https://github.com/ocornut/imgui/archive/docking.zip
  
)
FetchContent_MakeAvailable(imgui)