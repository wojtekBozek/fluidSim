cmake_minimum_required(VERSION 3.20)

# Include FetchContent to fetch external dependencies
include(FetchContent)

# Fetch and include GLEW, GLFW, GLM, stb_image, and ImGui
add_subdirectory(glew)
add_subdirectory(glm)
add_subdirectory(stb_image)
add_subdirectory(glfw)
add_subdirectory(imgui)
add_subdirectory(tinyobjloader)
# Include ImGui for glfw as a separate library
add_subdirectory(imgui_glfw)
add_subdirectory(native_file_dialog)