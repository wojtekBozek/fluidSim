#include <iostream>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "setup.hpp"
#include "mainWindow.hpp"
#include "simulationUI.hpp"

constexpr GLint WIDTH = 800, HEIGHT = 600;
enum class AppState {MAIN_MENU, SETTINGS};
AppState appState = AppState::MAIN_MENU;
int width = 800, height = 600;
int tempWidth = width, tempHeight = height;
bool stateChanged = false;

void showMainMenu()
{
    ImGui::Begin("MainMenu");
    if(ImGui::Button("Open Settings"))
    {
        stateChanged = false;
        tempWidth = width;
        tempHeight = height;
        appState = AppState::SETTINGS;
    }
    ImGui::End();
}

void showSettings(GLFWwindow* window)
{
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(ImVec2(200, 200));
    ImGui::Begin("Settings");
    //ImGui::End();
    //ImGui::Begin("Settings");

    ImGui::InputInt("Width: ", &tempWidth);
    ImGui::InputInt("Height: ", &tempHeight);

    tempWidth = std::clamp(tempWidth, 100, 1920);
    tempHeight = std::clamp(tempHeight, 100, 1080);
    if(ImGui::Button("Accept"))
    {
        width = tempWidth;
        height = tempHeight;
        glfwSetWindowSize(window, width, height);
        stateChanged = true;
    }

    if(!stateChanged)
    {
        if(ImGui::Button("Cancel"))
        {
            appState = AppState::MAIN_MENU;
        }
    }
    else
    {
        if(ImGui::Button("Back To Menu"))
        {
            appState = AppState::MAIN_MENU;
        }
    }
    ImGui::End();
}

int main()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    setup::setupGLFW();

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "First window", NULL, NULL);

    if (!window)
    { 
        printf("GLFWwindow creation failed!");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("Glew initialization failed!");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(appState == AppState::MAIN_MENU)
        {
            showMainMenu();
        }
        else if(appState == AppState::SETTINGS)
        {
            showSettings(window);
        }

        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}