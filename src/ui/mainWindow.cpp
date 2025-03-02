#include "mainWindow.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

MainWindow::MainWindow(GLFWwindow *window, std::shared_ptr<ProgramState> state) : m_window(window), m_state(state) {}

void MainWindow::showUI()
{
    if(*m_state == ProgramState::MAIN_MENU)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("MainMenu");
        chooseSimulationButton();
        exitButton();
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

void MainWindow::chooseSimulationButton()
{
    if(ImGui::Button("Choose simulation"))
    {
        *m_state = ProgramState::SIMULATION;
    }
}

void MainWindow::exitButton()
{
    if(ImGui::Button("Exit"))
    {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }
}