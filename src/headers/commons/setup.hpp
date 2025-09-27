#pragma once

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace  setup
{
    inline void setupGLFW(int majVersion=3, int minVersion=3, int coreProfile=GLFW_OPENGL_CORE_PROFILE, int forwardComp=GL_TRUE)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majVersion);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minVersion);
        glfwWindowHint(GLFW_OPENGL_PROFILE, coreProfile);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, forwardComp);
    }

    inline void setupImGui(GLFWwindow* window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO(); (void)io;
        ImGui_ImplGlfw_InitForOpenGL(window, true);  // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
        ImGui_ImplOpenGL3_Init("#version 130");
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
    }

    struct FrameBufferSize
    {
        int width;
        int height;
    };

    inline FrameBufferSize getWindowFrameBufferSize(GLFWwindow* window)
    {
        FrameBufferSize size = {0,0};
        glfwGetFramebufferSize(window, &size.width, &size.height);
        return size;
    }
}