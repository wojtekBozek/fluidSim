cmake_minimum_required(VERSION 3.14)

set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL 2 REQUIRED)

# Add the ImGui library with GLFW and OpenGL backend support
add_library(imgui_glfw STATIC
  imgui/imgui.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp

  imgui/backends/imgui_impl_glfw.cpp
  imgui/backends/imgui_impl_opengl2.cpp
)

# Link ImGui with glfw and OpenGL libraries
target_link_libraries(imgui_glfw PUBLIC glfw ${OPENGL_LIBRARIES})

# Set the include directories for ImGui and backends
target_include_directories(imgui_glfw
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/imgui
    ${CMAKE_CURRENT_LIST_DIR}/imgui/backends
)