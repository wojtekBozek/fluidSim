#include "simulationUI.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

SimulationUI::SimulationUI(GLFWwindow *window, std::shared_ptr<ProgramState> state) : m_window(window), m_state(state) {}

void SimulationUI::showUI()
{
    if(*m_state == ProgramState::SIMULATION)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Simulation settings");
        backToMenuButton();
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

void SimulationUI::backToMenuButton()
{
    if(ImGui::Button("Back To Menu"))
    {
        *m_state = ProgramState::MAIN_MENU;
    }
}
