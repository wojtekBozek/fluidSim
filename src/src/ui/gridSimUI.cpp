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
	particleShader();

	setTimeStep();
	setSimDim();
	setFluidDim();
	setFluidPos();
	setPressureIterations();
	setCellSize();
	setBorderSize();
	confirmChanges();

	returnToMenuButton();
	timeStepTime();
	frameRate();
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GridSimulationUI::setTimeStep()
{
	float value = refSimulation->getTimeStep();
	ImGui::InputFloat("TimeStep", &value, 0.0001f, 10.0f, "%.5f");
	if(value != refSimulation->getTimeStep())
	{
		refSimulation->setTimeStep(value);
	}
}

void GridSimulationUI::setSimDim()
{
	int sizeX = static_cast<int>(refSimulation->getSize().x);
	int sizeY = static_cast<int>(refSimulation->getSize().y);
	const int refSizeX = sizeX;
	const int refSizeY = sizeY;
	ImGui::InputInt("Simulation size (in cells) [X]", &sizeX, 16, 4096);
	ImGui::InputInt("Simulation size (in cells) [Y]", &sizeY, 16, 4096);
	if(sizeX != refSizeX ||  sizeY != refSizeY)
	{
		refSimulation->setDimensions(sizeX, sizeY);
	}
}

void GridSimulationUI::setFluidDim()
{
	int sizeX = static_cast<int>(refSimulation->getFluidSize().x);
	int sizeY = static_cast<int>(refSimulation->getFluidSize().y);
	const int refSizeX = sizeX;
	const int refSizeY = sizeY;
	ImGui::InputInt("Fluid size (cells) [X]", &sizeX, 16, refSimulation->getSize().x-refSimulation->getFluidPosition().x);
	ImGui::InputInt("Fluid size (cells) [Y]", &sizeY, 16, refSimulation->getSize().y-refSimulation->getFluidPosition().y);
	if(sizeX != refSizeX ||  sizeY != refSizeY)
	{
		refSimulation->setFluidSize(sizeX, sizeY);
	}
}

void GridSimulationUI::setFluidPos()
{
	int sizeX = static_cast<int>(refSimulation->getFluidPosition().x);
	int sizeY = static_cast<int>(refSimulation->getFluidPosition().y);
	const int refSizeX = sizeX;
	const int refSizeY = sizeY;
	ImGui::InputInt("Fluid init cell position [X]", &sizeX, 16, refSimulation->getSize().x-refSimulation->getFluidSize().x);
	ImGui::InputInt("Fluid init cell position [Y]", &sizeY, 16, refSimulation->getSize().y-refSimulation->getFluidSize().y);
	if(sizeX != refSizeX ||  sizeY != refSizeY)
	{
		refSimulation->setFluidInitPosition(sizeX, sizeY);
	}
}

void GridSimulationUI::setPressureIterations()
{
	int iterations = static_cast<int>(refSimulation->getPSolverIterations());
	const int ref = iterations;
	ImGui::InputInt("Pressure solver iterations", &iterations, 1, 1000);
	if(ref != iterations)
	{
		refSimulation->setPressureIterations(iterations);
	}
}

void GridSimulationUI::setCellSize()
{
	float dx =refSimulation->getDx();
	const float ref = dx;
	ImGui::InputFloat("Cell size [m]", &dx, 0.001, 100.0, "%.4f");
	if(ref != dx)
	{
		refSimulation->setCellSize(dx);
	}
}

void GridSimulationUI::setBorderSize()
{
	int size = static_cast<int>(refSimulation->getBorderSize());
	const int ref = size;
	ImGui::InputInt("Border size [in cells]", &size, 1, std::min(refSimulation->getFluidSize().x,refSimulation->getFluidSize().y));
	if(ref != size)
	{
		refSimulation->setPressureIterations(size);
	}
}

void GridSimulationUI::confirmChanges()
{
	if (ImGui::Button("ConfirmChanges"))
	{
		refSimulation->restart();
	}
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

void GridSimulationUI::particleShader()
{
	if (ImGui::Button("ParticleView"))
	{
		refRenderer->setParticleView();
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
