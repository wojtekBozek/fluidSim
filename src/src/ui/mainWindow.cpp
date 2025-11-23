#include "mainWindow.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

MainWindow::MainWindow(GLFWwindow* window, std::shared_ptr<ProgramState> state) : AbstractContext(window, state), m_menuState(MainMenuState::MAIN){}

void MainWindow::showUI()
{
    if(*m_state == ProgramState::MAIN_MENU)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("MainMenu");
        chooseSPHSimButton();
        chooseGRIDSimButton();
        exitButton();
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

void MainWindow::initContext()
{
}

void MainWindow::processContext(std::shared_ptr<rendering::Camera> camera)
{
}

void MainWindow::chooseSPHSimButton()
{
    if(ImGui::Button("SPH Simulation"))
    {
        *m_state = ProgramState::SPH_SIM;
    }
}

void MainWindow::chooseGRIDSimButton()
{
    if (ImGui::Button("Grid Simulation"))
    {
        *m_state = ProgramState::GRID_SIM;
    }
}

void MainWindow::exitButton()
{
    if(ImGui::Button("Exit"))
    {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }
}