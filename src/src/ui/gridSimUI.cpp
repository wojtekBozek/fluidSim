#include "gridSimUI.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
void GridSimulationUI::showUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("SPH-Simulation");
	returnToMenuButton();
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GridSimulationUI::returnToMenuButton()
{
	if (ImGui::Button("BackToMenu"))
	{
		*m_state = ProgramState::MAIN_MENU;
	}
}
