cmake_minimum_required(VERSION 3.20)

project(FluidSimulator)

set(IMGUI_VERSION 1.92)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()
add_subdirectory(external)

file(GLOB_RECURSE _main CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_executable(${PROJECT_NAME} ${_main}
${CMAKE_SOURCE_DIR}/src/src/commons/shader.cpp
${CMAKE_SOURCE_DIR}/src/src/commons/shaderProgram.cpp
${CMAKE_SOURCE_DIR}/src/src/geometry/vertex.cpp 
${CMAKE_SOURCE_DIR}/src/src/geometry/mesh.cpp 
${CMAKE_SOURCE_DIR}/src/src/camera/camera3D.cpp 
${CMAKE_SOURCE_DIR}/src/src/objects/object.cpp 
${CMAKE_SOURCE_DIR}/src/src/objects/objectsMeneger.cpp 
${CMAKE_SOURCE_DIR}/src/src/program/sceneLoader.cpp 
${CMAKE_SOURCE_DIR}/src/src/program/mesh_loader.cpp 
${CMAKE_SOURCE_DIR}/src/src/program/particleRenderer.cpp 
${CMAKE_SOURCE_DIR}/src/src/properties/physics.cpp
${CMAKE_SOURCE_DIR}/src/src/properties/FluidSPHSimulation.cpp 
${CMAKE_SOURCE_DIR}/src/src/properties/FluidRenderer.cpp 
${CMAKE_SOURCE_DIR}/src/src/ui/mainWindow.cpp 
${CMAKE_SOURCE_DIR}/src/src/ui/simulationUI.cpp
${CMAKE_SOURCE_DIR}/src/src/program/application.cpp
${CMAKE_SOURCE_DIR}/src/src/camera/perspectiveCamera.cpp 
${CMAKE_SOURCE_DIR}/src/src/camera/orthographicCamera.cpp 
${CMAKE_SOURCE_DIR}/src/src/program/context.cpp
${CMAKE_SOURCE_DIR}/src/src/ui/sphSimulationUI.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE 
${CMAKE_SOURCE_DIR}/src/headers/camera/
${CMAKE_SOURCE_DIR}/src/headers/geometry/
${CMAKE_SOURCE_DIR}/src/headers/math/
${CMAKE_SOURCE_DIR}/src/headers/objects/
${CMAKE_SOURCE_DIR}/src/headers/commons/
${CMAKE_SOURCE_DIR}/src/headers/program/
${CMAKE_SOURCE_DIR}/src/headers/properties/
${CMAKE_SOURCE_DIR}/src/headers/ui/
${CMAKE_SOURCE_DIR}/src/headers/lights/
)

add_definitions(-DGLEW_STATIC)


if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32 glfw libglew_static glm stb_image imgui_glfw tinyobjloader nfd)
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE GL glfw libglew_static glm stb_image imgui_glfw tinyobjloader nfd)
endif()



target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

option(ENABLE_ASAN "Enable AddressSanitizer (runtime memory error detection)" ON)

if (ENABLE_ASAN)
    message(STATUS "Enabling AddressSanitizer")

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # GCC and Clang
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)

    elseif (MSVC)
        # MSVC (Visual Studio 2019+ with /fsanitize=address support)
        add_compile_options(/fsanitize=address)
        add_link_options(/fsanitize=address)

    else()
        message(WARNING "AddressSanitizer not supported on this compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()

# Copy shader files to the build folder
# configure_file(${SHADERS_DIR}/vertex.shader ${CMAKE_BINARY_DIR}/shaders/vertex.shader COPYONLY)
# configure_file(${SHADERS_DIR}/fragment.shader ${CMAKE_BINARY_DIR}/shaders/fragment.shader COPYONLY)

# more elaborate way to copy shaders
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})
if (EXISTS "${CMAKE_SOURCE_DIR}/obj")
    file(COPY ${CMAKE_SOURCE_DIR}/obj DESTINATION ${CMAKE_BINARY_DIR})
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)

if (EXISTS "${CMAKE_SOURCE_DIR}/obj")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/obj
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/obj
    )
endif()