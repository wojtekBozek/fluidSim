#include "mixedSimUI.hpp"
#include <algorithm>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "SPHContextState.hpp"

void MixedSimulationUI::showUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("PIC-FLIP Simulation");
	startSimulation();
	resetSimulation();
	vVelocityShader();
	uVelocityShader();
	divergenceShader();
	pressureShader();
	visualShader();
	particleShader();
	setDensity();

	setPicFlipRatio();
	setOverrelaxation();
	setPressurePolicy();
	setSolver();
	setTimeStep();
	setSimDim();
	setFluidDim();
	setFluidPos();
	setPressureIterations();
	setNumOfParticlesPerCell();
	setCellSize();
	setBorderSize();
	confirmChanges();

	returnToMenuButton();
	timeStepTime();
	displayCurrentStep();
	frameRate();
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MixedSimulationUI::setTimeStep()
{
	float value = refSimulation->getTimeStep();
	ImGui::InputFloat("TimeStep", &value, 0.0001f, 0.01f, "%.4f");
	value = std::clamp(value, 0.00001f, 10.0f);
	if(value != refSimulation->getTimeStep())
	{
		refSimulation->setTimeStep(value);
	}
}

void MixedSimulationUI::setSimDim()
{
	int sizeX = static_cast<int>(refSimulation->getSize().x);
	int sizeY = static_cast<int>(refSimulation->getSize().y);
	const int refSizeX = sizeX;
	const int refSizeY = sizeY;
	ImGui::InputInt("Simulation size (in cells) [X]", &sizeX, 1, 10);
	ImGui::InputInt("Simulation size (in cells) [Y]", &sizeY, 1, 10);
	sizeX = std::clamp(sizeX, 16, 4096);
	sizeY = std::clamp(sizeY, 16, 4096);
	if(sizeX != refSizeX ||  sizeY != refSizeY)
	{
		refSimulation->setDimensions(sizeX, sizeY);
	}
}

void MixedSimulationUI::setFluidDim()
{
	int sizeX = static_cast<int>(refSimulation->getFluidSize().x);
	int sizeY = static_cast<int>(refSimulation->getFluidSize().y);
	const int refSizeX = sizeX;
	const int refSizeY = sizeY;
	ImGui::InputInt("Fluid size (cells) [X]", &sizeX, 1, 10);
	ImGui::InputInt("Fluid size (cells) [Y]", &sizeY, 1, 10);
	sizeX = std::clamp(sizeX, 1, int(refSimulation->getSize().x-refSimulation->getFluidPosition().x));
	sizeY = std::clamp(sizeY, 1, int(refSimulation->getSize().y-refSimulation->getFluidPosition().y));
	if(sizeX != refSizeX ||  sizeY != refSizeY)
	{
		refSimulation->setFluidSize(sizeX, sizeY);
	}
}

void MixedSimulationUI::setFluidPos()
{
	int sizeX = static_cast<int>(refSimulation->getFluidPosition().x);
	int sizeY = static_cast<int>(refSimulation->getFluidPosition().y);
	const int refSizeX = sizeX;
	const int refSizeY = sizeY;
	ImGui::InputInt("Fluid init cell position [X]", &sizeX, 1, 10);
	ImGui::InputInt("Fluid init cell position [Y]", &sizeY, 1, 10);
	sizeX = std::clamp(sizeX, 1, int(refSimulation->getSize().x-refSimulation->getFluidSize().x));
	sizeY = std::clamp(sizeY, 1, int(refSimulation->getSize().y-refSimulation->getFluidSize().y));
	if(sizeX != refSizeX ||  sizeY != refSizeY)
	{
		refSimulation->setFluidInitPosition(sizeX, sizeY);
	}
}

void MixedSimulationUI::setPressureIterations()
{
	int iterations = static_cast<int>(refSimulation->getPSolverIterations());
	const int ref = iterations;
	ImGui::InputInt("Pressure solver iterations", &iterations, 1, 10);
	
	iterations = std::clamp(iterations, 1, 1000);
	if(ref != iterations)
	{
		refSimulation->setPressureIterations(iterations);
	}
}

void MixedSimulationUI::setCellSize()
{
	float dx =refSimulation->getDx();
	const float ref = dx;
	ImGui::InputFloat("Cell size [m]", &dx, 0.001, 0.1, "%.4f");
	
	dx = std::clamp(dx, 0.0001f, 100.0f);
	if(ref != dx)
	{
		refSimulation->setCellSize(dx);
	}
}

void MixedSimulationUI::setBorderSize()
{
	int size = static_cast<int>(refSimulation->getBorderSize());
	const int ref = size;
	ImGui::InputInt("Border size [in cells]", &size, 1, 5);
	size = std::clamp(size, 1, int(std::min(refSimulation->getFluidSize().x,refSimulation->getFluidSize().y)));
	if(ref != size)
	{
		refSimulation->setBorderSize(size);
	}
}

void MixedSimulationUI::displayCurrentStep()
{
	ImGui::Value("CurrentStep", static_cast<unsigned int>(refSimulation->getCurrentStep()));
}

void MixedSimulationUI::setPicFlipRatio()
{
	float ratio =refSimulation->getPicFlipRatio();
	const float ref = ratio;
	ImGui::SliderFloat("Pic-Flip Ratio", &ratio, 0.0f, 1.0f, "%.2f");
	if(ref != ratio)
	{
		refSimulation->setPicFlipRatio(ratio);
	}
}

void MixedSimulationUI::setDensity()
{
	float dens =refSimulation->getDensity();
	const float ref = dens;
	ImGui::InputFloat("Density", &dens, 0.1f, 0.1f, "%.2f");
	dens = std::clamp(dens, 1.0f, 10000.0f);
	if(ref != dens)
	{
		refSimulation->setDensity(dens);
	}
}

void MixedSimulationUI::setSolver()
{
	Grid2D::SOLVER solver = refSimulation->getSolver();
	const Grid2D::SOLVER ref = solver;
	const char* labels[] = { "Jacobi Solver", "GS_SOLVER", "GS_SIMPLIFIEDSOLVER" };

	ImGui::Combo("Solver", (int*)&solver, labels, IM_ARRAYSIZE(labels));
	if(ref != solver)
	{
		refSimulation->setSolver(solver);
	}
}

void MixedSimulationUI::setPressurePolicy()
{
	bool solver = refSimulation->getPressurePolicy();
	const bool ref = solver;
	const char* labels[] = {"Keep pressure as initial guess", "Zero Pressure as initial guess", };

	ImGui::Combo("Pressure policy", (int*)&solver, labels, IM_ARRAYSIZE(labels));
	if(ref != solver)
	{
		refSimulation->setPressureZero(solver);
	}
}

void MixedSimulationUI::setOverrelaxation()
{
	float overrelaxation =refSimulation->getOverrelaxation();
	const float ref = overrelaxation;
	const Grid2D::SOLVER refSolver = refSimulation->getSolver();
	const float maxOverrelaxation = refSolver == Grid2D::SOLVER::JACOBI ? 1.0f : 2.0f;
	
	const float minOverrelaxation = refSolver == Grid2D::SOLVER::JACOBI ? 0.0f : 1.0f;
	ImGui::InputFloat("Overrelaxation [m]", &overrelaxation, 0.1f, 0.1f, "%.2f");
	overrelaxation = std::clamp(overrelaxation, minOverrelaxation, maxOverrelaxation);
	if(ref != overrelaxation)
	{
		refSimulation->setOverrelaxation(overrelaxation);
	}
}

void MixedSimulationUI::setNumOfParticlesPerCell()
{
	int num = static_cast<int>(refSimulation->getParticlesPerCell());
	const int ref = num;
	ImGui::InputInt("Particles per cell", &num, 1, 1);
	num = std::clamp(num, 1, 255);
	if(ref != num)
	{
		refSimulation->setParticlesPerCell(num);
	}
}

void MixedSimulationUI::confirmChanges()
{
	if (ImGui::Button("ConfirmChanges"))
	{
		refSimulation->restart();
	}
}

void MixedSimulationUI::returnToMenuButton()
{
	if (ImGui::Button("BackToMenu"))
	{
		*m_state = ProgramState::MAIN_MENU;
	}
}

void MixedSimulationUI::startSimulation()
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

void MixedSimulationUI::vVelocityShader()
{
	if (ImGui::Button("VVelocityView"))
	{
		refRenderer->setVVelTexActive();
	}
}

void MixedSimulationUI::uVelocityShader()
{
	if (ImGui::Button("uVelocityView"))
	{
		refRenderer->setUVelTexActive();
	}
}

void MixedSimulationUI::divergenceShader()
{
	if (ImGui::Button("DivergenceView"))
	{
		refRenderer->setdivergenceTexActive();
	}
}

void MixedSimulationUI::pressureShader()
{
	if (ImGui::Button("PressureView"))
	{
		refRenderer->setPressureTexActive();
	}
}

void MixedSimulationUI::visualShader()
{
	if (ImGui::Button("TypeView"))
	{
		refRenderer->setDefaultView();
	}
}

void MixedSimulationUI::particleShader()
{
	if (ImGui::Button("ParticleView"))
	{
		refRenderer->setParticleView();
	}
}

void MixedSimulationUI::timeStepTime()
{
	ImGui::Value("ComputeTime [ns]", refSimulation->getComputeTime(), "%.001f");
}

void MixedSimulationUI::frameRate()
{
	float fr = 1.0f / (static_cast<float>(refSimulation->getComputeTime())/1000000000.000f);

	ImGui::Value("FrameRate", fr, "%.001f");
}

void MixedSimulationUI::resetSimulation()
{
	if (ImGui::Button("Reset"))
	{
		refSimulation->restart();
	}
}
