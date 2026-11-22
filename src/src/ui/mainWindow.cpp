#include "mainWindow.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

MainWindow::MainWindow(GLFWwindow* window, std::shared_ptr<ProgramState> state) : AbstractContext(window, state), m_menuState(MainMenuState::MAIN) {    
    ui = std::make_shared<InternalUI>(window, state);
}

void MainWindow::InternalUI::show()
{
    if(*m_state == ProgramState::MAIN_MENU)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("MainMenu", nullptr, flags);
        chooseSPHSimButton();
        chooseGRIDSimButton(); 
        chooseMixedSimButton();
        chooseGRID3dSimButton();
        texture3DTestButton();
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

void MainWindow::InternalUI::chooseSPHSimButton()
{
    if(ImGui::Button("SPH Simulation"))
    {
        *m_state = ProgramState::SPH_SIM;
    }
}

void MainWindow::InternalUI::chooseGRIDSimButton()
{
    if (ImGui::Button("Grid Simulation"))
    {
        *m_state = ProgramState::GRID_SIM;
    }
}

void MainWindow::InternalUI::chooseGRID3dSimButton()
{
    if (ImGui::Button("Grid Simulation 3D"))
    {
        *m_state = ProgramState::GRID_SIM_3D;
    }
}

void MainWindow::InternalUI::textureTestButton()
{
    if (ImGui::Button("Texture Test"))
    {
        *m_state = ProgramState::TEXTURE_TEST;
    }
}

void MainWindow::InternalUI::texture3DTestButton()
{
    if (ImGui::Button("3D Texture Test"))
    {
        *m_state = ProgramState::TEXTURE_TEST3D;
    }
}

void MainWindow::InternalUI::chooseMixedSimButton()
{
    if (ImGui::Button("PIC-APIC simulation"))
    {
        *m_state = ProgramState::MIXED_SIM;
    }
}

void MainWindow::InternalUI::exitButton()
{
    if(ImGui::Button("Exit"))
    {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }
}