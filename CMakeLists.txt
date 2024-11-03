cmake_minimum_required(VERSION 3.14)

project(FluidSimulator)

set(IMGUI_VERSION 1.87)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

add_subdirectory(external)

# Set the path to the shaders folder relative to the CMakeLists.txt file
set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

file(GLOB_RECURSE _main CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_executable(${PROJECT_NAME} ${_main}
${CMAKE_SOURCE_DIR}/src/commons/shader.cpp
${CMAKE_SOURCE_DIR}/src/commons/shaderProgram.cpp
${CMAKE_SOURCE_DIR}/src/geometry/vertex.cpp 
${CMAKE_SOURCE_DIR}/src/geometry/mesh.cpp )

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/headers)

add_definitions(-DGLEW_STATIC)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32 glfw libglew_static glm stb_image imgui_glfw tinyobjloader)
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} GL glfw libglew_static glm stb_image imgui_glfw tinyobjloader)
endif()


# Copy shader files to the build folder
# configure_file(${SHADERS_DIR}/vertex.shader ${CMAKE_BINARY_DIR}/shaders/vertex.shader COPYONLY)
# configure_file(${SHADERS_DIR}/fragment.shader ${CMAKE_BINARY_DIR}/shaders/fragment.shader COPYONLY)

# more elaborate way to copy shaders
file(COPY ${SHADERS_DIR} DESTINATION ${CMAKE_BINARY_DIR}/shaders)
