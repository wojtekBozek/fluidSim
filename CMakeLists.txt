cmake_minimum_required(VERSION 3.7.2)

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
${CMAKE_SOURCE_DIR}/src/geometry/mesh.cpp 
${CMAKE_SOURCE_DIR}/src/camera/camera3D.cpp 
${CMAKE_SOURCE_DIR}/src/objects/object.cpp 
${CMAKE_SOURCE_DIR}/src/objects/objectsMeneger.cpp 
${CMAKE_SOURCE_DIR}/src/program/sceneLoader.cpp 
${CMAKE_SOURCE_DIR}/src/program/mesh_loader.cpp 
${CMAKE_SOURCE_DIR}/src/properties/physics.cpp
${CMAKE_SOURCE_DIR}/src/properties/particles.cpp 
${CMAKE_SOURCE_DIR}/src/ui/mainWindow.cpp 
${CMAKE_SOURCE_DIR}/src/ui/simulationUI.cpp 
)

target_include_directories(${PROJECT_NAME} PRIVATE 
${CMAKE_SOURCE_DIR}/src/headers/camera/
${CMAKE_SOURCE_DIR}/src/headers/geometry/
${CMAKE_SOURCE_DIR}/src/headers/math/
${CMAKE_SOURCE_DIR}/src/headers/objects/
${CMAKE_SOURCE_DIR}/src/headers/commons/
${CMAKE_SOURCE_DIR}/src/headers/program/
${CMAKE_SOURCE_DIR}/src/headers/properties/
${CMAKE_SOURCE_DIR}/src/headers/ui/
)

add_definitions(-DGLEW_STATIC)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32 glfw libglew_static glm stb_image imgui_glfw tinyobjloader)
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} GL glfw libglew_static glm stb_image imgui_glfw tinyobjloader)
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Copy shader files to the build folder
# configure_file(${SHADERS_DIR}/vertex.shader ${CMAKE_BINARY_DIR}/shaders/vertex.shader COPYONLY)
# configure_file(${SHADERS_DIR}/fragment.shader ${CMAKE_BINARY_DIR}/shaders/fragment.shader COPYONLY)

# more elaborate way to copy shaders
file(COPY ${SHADERS_DIR} DESTINATION ${CMAKE_BINARY_DIR}/shaders)
