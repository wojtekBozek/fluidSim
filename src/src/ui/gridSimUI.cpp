#include "gridSimUI.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "SPHContextState.hpp"

void GridSimulationUI::showUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("SPH-Simulation");
	startSimulation();
	resetSimulation();
	vVelocityShader();
	uVelocityShader();
	divergenceShader();
	pressureShader();
	visualShader();
	returnToMenuButton();
	timeStepTime();
	frameRate();
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

void GridSimulationUI::startSimulation()
{
	if (*refContext != ContextState::RUNNING)
	{
		if (ImGui::Button("StartSimulation"))
		{
			*refContext = ContextState::RUNNING;
		}	
	}
	else
	{
		if (ImGui::Button("StopSimulation"))
		{
			*refContext = ContextState::SETUP;
		}
	}
}

void GridSimulationUI::vVelocityShader()
{
	if (ImGui::Button("VVelocityView"))
	{
		refRenderer->setVVelTexActive();
	}
}

void GridSimulationUI::uVelocityShader()
{
	if (ImGui::Button("uVelocityView"))
	{
		refRenderer->setUVelTexActive();
	}
}

void GridSimulationUI::divergenceShader()
{
	if (ImGui::Button("DivergenceView"))
	{
		refRenderer->setdivergenceTexActive();
	}
}

void GridSimulationUI::pressureShader()
{
	if (ImGui::Button("PressureView"))
	{
		refRenderer->setPressureTexActive();
	}
}

void GridSimulationUI::visualShader()
{
	if (ImGui::Button("TypeView"))
	{
		refRenderer->setDefaultView();
	}
}

void GridSimulationUI::timeStepTime()
{
	ImGui::Value("ComputeTime [ns]", refSimulation->getComputeTime(), "%.001f");

}

void GridSimulationUI::frameRate()
{
	float fr = 1.0f / (static_cast<float>(refSimulation->getComputeTime())/1000000000.000f);

	ImGui::Value("FrameRate", fr, "%.001f");
}

void GridSimulationUI::resetSimulation()
{
	if (ImGui::Button("Reset"))
	{
		refSimulation->restart();
	}
}
