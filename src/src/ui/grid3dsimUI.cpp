#include "gridSim3dUI.hpp"
#include <algorithm>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "SPHContextState.hpp"

void Grid3dSimulationUI::showUI()
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
	setDensity();

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
	frameRate();
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Grid3dSimulationUI::setTimeStep()
{
	float value = refSimulation->getTimeStep();
	ImGui::InputFloat("TimeStep", &value, 0.0001f, 0.01f, "%.4f");
	value = std::clamp(value, 0.00001f, 10.0f);
	if(value != refSimulation->getTimeStep())
	{
		refSimulation->setTimeStep(value);
	}
}

void Grid3dSimulationUI::setSimDim()
{
	int sizeX = static_cast<int>(refSimulation->getSize().x);
	int sizeY = static_cast<int>(refSimulation->getSize().y);
	int sizeZ = static_cast<int>(refSimulation->getSize().z);
	const int refSizeX = sizeX;
	const int refSizeY = sizeY;
	const int refSizeZ = sizeZ;
	ImGui::InputInt("Simulation size (in cells) [X]", &sizeX, 1, 10);
	ImGui::InputInt("Simulation size (in cells) [Y]", &sizeY, 1, 10);
	ImGui::InputInt("Simulation size (in cells) [Z]", &sizeZ, 1, 10);
	sizeX = std::clamp(sizeX, 16, 4096);
	sizeY = std::clamp(sizeY, 16, 4096);
	sizeZ = std::clamp(sizeZ, 16, 4096);
	if(sizeX != refSizeX ||  sizeY != refSizeY ||  sizeZ != refSizeZ)
	{
		refSimulation->setDimensions(sizeX, sizeY, sizeZ);
	}
}

void Grid3dSimulationUI::setFluidDim()
{
	int sizeX = static_cast<int>(refSimulation->getFluidSize().x);
	int sizeY = static_cast<int>(refSimulation->getFluidSize().y);
	int sizeZ = static_cast<int>(refSimulation->getFluidSize().z);
	const int refSizeX = sizeX;
	const int refSizeY = sizeY;
	const int refSizeZ = sizeZ;
	ImGui::InputInt("Fluid size (cells) [X]", &sizeX, 1, 10);
	ImGui::InputInt("Fluid size (cells) [Y]", &sizeY, 1, 10);
	ImGui::InputInt("Fluid size (cells) [Z]", &sizeZ, 1, 10);
	sizeX = std::clamp(sizeX, 1, int(refSimulation->getSize().x-refSimulation->getFluidPosition().x));
	sizeY = std::clamp(sizeY, 1, int(refSimulation->getSize().y-refSimulation->getFluidPosition().y));
	sizeZ = std::clamp(sizeZ, 1, int(refSimulation->getSize().y-refSimulation->getFluidPosition().z));
	if(sizeX != refSizeX ||  sizeY != refSizeY ||  sizeZ != refSizeZ)
	{
		refSimulation->setFluidSize(sizeX, sizeY, sizeZ);
	}
}

void Grid3dSimulationUI::setFluidPos()
{
	int sizeX = static_cast<int>(refSimulation->getFluidPosition().x);
	int sizeY = static_cast<int>(refSimulation->getFluidPosition().y);
	int sizeZ = static_cast<int>(refSimulation->getFluidPosition().z);
	const int refSizeX = sizeX;
	const int refSizeY = sizeY;
	const int refSizeZ = sizeZ;
	ImGui::InputInt("Fluid init cell position [X]", &sizeX, 1, 10);
	ImGui::InputInt("Fluid init cell position [Y]", &sizeY, 1, 10);
	ImGui::InputInt("Fluid init cell position [Z]", &sizeZ, 1, 10);
	sizeX = std::clamp(sizeX, 1, int(refSimulation->getSize().x-refSimulation->getFluidSize().x));
	sizeY = std::clamp(sizeY, 1, int(refSimulation->getSize().y-refSimulation->getFluidSize().y));
	sizeZ = std::clamp(sizeZ, 1, int(refSimulation->getSize().y-refSimulation->getFluidSize().z));
	if(sizeX != refSizeX ||  sizeY != refSizeY ||  sizeZ != refSizeZ)
	{
		refSimulation->setFluidInitPosition(sizeX, sizeY, sizeZ);
	}
}

void Grid3dSimulationUI::setPressureIterations()
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

void Grid3dSimulationUI::setCellSize()
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

void Grid3dSimulationUI::setBorderSize()
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

void Grid3dSimulationUI::setDensity()
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

void Grid3dSimulationUI::setSolver()
{
	Grid3D::SOLVER solver = refSimulation->getSolver();
	const Grid3D::SOLVER ref = solver;
	const char* labels[] = { "Jacobi Solver", "GS_SOLVER", "GS_SIMPLIFIEDSOLVER" };

	ImGui::Combo("Solver", (int*)&solver, labels, IM_ARRAYSIZE(labels));
	if(ref != solver)
	{
		refSimulation->setSolver(solver);
	}
}

void Grid3dSimulationUI::setPressurePolicy()
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

void Grid3dSimulationUI::setOverrelaxation()
{
	float overrelaxation =refSimulation->getOverrelaxation();
	const float ref = overrelaxation;
	const Grid3D::SOLVER refSolver = refSimulation->getSolver();
	const float maxOverrelaxation = refSolver == Grid3D::SOLVER::JACOBI ? 1.0f : 2.0f;
	
	const float minOverrelaxation = refSolver == Grid3D::SOLVER::JACOBI ? 0.0f : 1.0f;
	ImGui::InputFloat("Overrelaxation [m]", &overrelaxation, 0.1f, 0.1f, "%.2f");
	overrelaxation = std::clamp(overrelaxation, minOverrelaxation, maxOverrelaxation);
	if(ref != overrelaxation)
	{
		refSimulation->setOverrelaxation(overrelaxation);
	}
}

void Grid3dSimulationUI::setNumOfParticlesPerCell()
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

void Grid3dSimulationUI::confirmChanges()
{
	if (ImGui::Button("ConfirmChanges"))
	{
		refSimulation->restart();
	}
}

void Grid3dSimulationUI::returnToMenuButton()
{
	if (ImGui::Button("BackToMenu"))
	{
		*m_state = ProgramState::MAIN_MENU;
	}
}

void Grid3dSimulationUI::startSimulation()
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

void Grid3dSimulationUI::vVelocityShader()
{
	if (ImGui::Button("VVelocityView"))
	{
		refRenderer->setVVelTexActive();
	}
}

void Grid3dSimulationUI::uVelocityShader()
{
	if (ImGui::Button("uVelocityView"))
	{
		refRenderer->setUVelTexActive();
	}
}

void Grid3dSimulationUI::divergenceShader()
{
	if (ImGui::Button("DivergenceView"))
	{
		refRenderer->setdivergenceTexActive();
	}
}

void Grid3dSimulationUI::pressureShader()
{
	if (ImGui::Button("PressureView"))
	{
		refRenderer->setPressureTexActive();
	}
}

void Grid3dSimulationUI::visualShader()
{
	if (ImGui::Button("TypeView"))
	{
		refRenderer->setDefaultView();
	}
}

void Grid3dSimulationUI::particleShader()
{
	if (ImGui::Button("ParticleView"))
	{
		refRenderer->setParticleView();
	}
}

void Grid3dSimulationUI::timeStepTime()
{
	ImGui::Value("ComputeTime [ns]", refSimulation->getComputeTime(), "%.001f");
}

void Grid3dSimulationUI::frameRate()
{
	float fr = 1.0f / (static_cast<float>(refSimulation->getComputeTime())/1000000000.000f);

	ImGui::Value("FrameRate", fr, "%.001f");
}

void Grid3dSimulationUI::resetSimulation()
{
	if (ImGui::Button("Reset"))
	{
		refSimulation->restart();
	}
}
