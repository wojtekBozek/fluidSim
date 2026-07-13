#pragma once

#include "context.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
class Texture3DTestContext : public AbstractContext
{
public:
    Texture3DTestContext(GLFWwindow* window,
                         std::shared_ptr<ProgramState> state);

    ~Texture3DTestContext();

    void initContext() override;
    void processContext(std::shared_ptr<rendering::Camera> camera) override;

    void exitButton();
	void showUI() override;
private:
    GLuint texture3D = 0;
    GLuint computeProgram = 0;

    GLint deltaLocation = -1;

    bool executed = false;

    GLuint compileShader(GLenum type, const char* source);
    GLuint createComputeProgram();
};