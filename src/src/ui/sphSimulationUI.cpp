#include "sphSimulationUI.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


void SPHsimulationUI::showUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("SPH-Simulation");
	fluidStiffKcoeffTextInput();
	fluidStiffYcoeffTextInput();

	if (*simulationContext != ContextState::RUNNING) 
	{
		fluidDensityTextInput();
		dimensionComboBox();
		fluidVolumeTextInputs();
		fluidPositionTextInputs();
		simulationVolumeTextInputs();
		simulationPositionTextInputs();
	}
	sphKernelRadiusSizeInput();
	visualRadiusSizeInput();
	timeStepInput();
	numOfParticlesTextInfo();
	volumeTextInfo();
	nsComputationTextInfo();
	frameRateTextInfo();

	startSimulationButton();
	restartSimulationButton();
	stopSimulationButton();
	pauseSimulationButton();

	returnToMenuButton();
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SPHsimulationUI::fluidStiffKcoeffTextInput()
{
	float PrevValue = refSimulation->getFluid().stiffnesK;
	uiValues.k = PrevValue;
	ImGui::InputFloat("StiffnessK", &uiValues.k, 0.0001f, 10000.0f, "%.5f");

	if (uiValues.k != PrevValue)
	{
		refSimulation->setFluidStiffnessKCoefficient(uiValues.k);
	}
}

void SPHsimulationUI::fluidStiffYcoeffTextInput()
{
	float PrevValue = refSimulation->getFluid().stiffnesY;
	uiValues.y = PrevValue;
	ImGui::InputFloat("StiffnessY", &uiValues.y, 1.0f, 15.0f, "%.3f");

	if (uiValues.y != PrevValue)
	{
		refSimulation->setFluidStiffnessYCoefficient(uiValues.y);
	}
}

void SPHsimulationUI::fluidDensityTextInput()
{
	float PrevValue = refSimulation->getFluid().fluidDensity;
	uiValues.density = PrevValue;
	ImGui::InputFloat("Density", &uiValues.density, 0.0f, 10000000.0f, "%.4f");

	if (uiValues.density != PrevValue)
	{
		refSimulation->setFluidDensity(uiValues.density);
	}
}

void SPHsimulationUI::dimensionComboBox()
{
	if (refSimulation->getSimulationDimension() == DIMENSION_2)
	{
		if (ImGui::Button("Set 3D dim"))
		{
			refSimulation->setSimulationDimension(DIMENSION_3);
			refSimulation->clearSimulation();
			refSimulation->setInitialSimulationDomain();
			refSimulation->setSimulationState();
			refSimulation->setFluidAndParticles();
			refSimulation->setMemoryLayout();
		}	
	}
	else if(refSimulation->getSimulationDimension() == DIMENSION_3)
	{
		if (ImGui::Button("Set 2D dim"))
		{
			refSimulation->setSimulationDimension(DIMENSION_2);
			refSimulation->clearSimulation();
			refSimulation->setInitialSimulationDomain();
			refSimulation->setSimulationState();
			refSimulation->setFluidAndParticles();
			refSimulation->setMemoryLayout();
		}
	}
}

void SPHsimulationUI::fluidVolumeTextInputs()
{
	glm::vec3 PrevValue = refSimulation->getFluidDomain().size;
	uiValues.fluidInitSize = PrevValue;
	ImGui::InputFloat("SimFluidSizeX", &uiValues.fluidInitSize.x, 0.0f, 10000000.0f, "%.4f");
	ImGui::InputFloat("SimFluidSizeY", &uiValues.fluidInitSize.y, 0.0f, 10000000.0f, "%.4f");
	if(refSimulation->getSimulationDimension() == SimDim::DIMENSION_3)
		ImGui::InputFloat("SimFluidSizeZ", &uiValues.fluidInitSize.z, 0.0f, 10000000.0f, "%.4f");

	//if (uiValues.fluidInitSize != PrevValue)
	//{
		refSimulation->setFluidDomainSize(uiValues.fluidInitSize);
	//}
}

void SPHsimulationUI::fluidPositionTextInputs()
{
	glm::vec3 PrevValue = refSimulation->getFluidDomain().position;
	uiValues.fluidInitPos = PrevValue;
	ImGui::InputFloat("fluidInitPosX", &uiValues.fluidInitPos.x, -10000000.0f, 10000000.0f, "%.4f");
	ImGui::InputFloat("fluidInitPosY", &uiValues.fluidInitPos.y, -10000000.0f, 10000000.0f, "%.4f");
	if (refSimulation->getSimulationDimension() == SimDim::DIMENSION_3)
		ImGui::InputFloat("fluidInitPosZ", &uiValues.fluidInitPos.z, -10000000.0f, 10000000.0f, "%.4f");

	//if (uiValues.fluidInitPos != PrevValue)
	//{
		refSimulation->setFluidDomainPosition(uiValues.fluidInitPos);
	//}
}

void SPHsimulationUI::simulationVolumeTextInputs()
{
	glm::vec3 PrevValue = refSimulation->getSimulationDomain().size;
	uiValues.simDomainSize = PrevValue;
	ImGui::InputFloat("SimDomainSizeX", &uiValues.simDomainSize.x, 0.0f, 10000000.0f, "%.4f");
	ImGui::InputFloat("SimDomainSizeY", &uiValues.simDomainSize.y, 0.0f, 10000000.0f, "%.4f");
	if (refSimulation->getSimulationDimension() == SimDim::DIMENSION_3)
		ImGui::InputFloat("SimDomainSizeZ", &uiValues.simDomainSize.z, 0.0f, 10000000.0f, "%.4f");

	//if (uiValues.simDomainSize != PrevValue)
	//{
		refSimulation->setSimuilationDomainSize(uiValues.simDomainSize);
	//}
}

void SPHsimulationUI::simulationPositionTextInputs()
{
	glm::vec3 PrevValue = refSimulation->getSimulationDomain().position;
	uiValues.simDomainPos = PrevValue;
	ImGui::InputFloat("simDomainPosX", &uiValues.simDomainPos.x, -10000000.0f, 10000000.0f, "%.3f");
	ImGui::InputFloat("simDomainPosY", &uiValues.simDomainPos.y, -10000000.0f, 10000000.0f, "%.3f");
	if (refSimulation->getSimulationDimension() == SimDim::DIMENSION_3)
		ImGui::InputFloat("simDomainPosZ", &uiValues.simDomainPos.z, -10000000.0f, 10000000.0f, "%.3f");
	//if (uiValues.simDomainPos != PrevValue)
	//{
		refSimulation->setSimulationsDomainPosition(uiValues.simDomainPos);
	//}
}

void SPHsimulationUI::sphKernelRadiusSizeInput()
{
	float PrevValue = refSimulation->getKernelRadius();
	uiValues.kernelRadius = PrevValue;
	ImGui::InputFloat("kernelRadius", &uiValues.kernelRadius, 0.001f, 10.0f, "%.3f");

	if (uiValues.kernelRadius != PrevValue)
	{
		refSimulation->setKernelRadius(uiValues.kernelRadius);
	}
}

void SPHsimulationUI::visualRadiusSizeInput()
{
	float PrevValue = refSimulation->getParticleRadius();
	uiValues.visualRadius = PrevValue;
	ImGui::InputFloat("ParticleRadius", &uiValues.visualRadius, 0.001f, 10.0f, "%.3f");

	if (uiValues.visualRadius != PrevValue)
	{
		refSimulation->setParticleRadius(uiValues.visualRadius);
	}
}

void SPHsimulationUI::numOfParticlesTextInfo()
{
	volatile uint32_t value = refSimulation->getNumOfParticles();
	ImGui::Value("NumOfParticles", value);
}

void SPHsimulationUI::volumeTextInfo()
{
	ImGui::Value("FluidVolume", refSimulation->getFluid().volume, "%.3f");
}

void SPHsimulationUI::nsComputationTextInfo()
{
	ImGui::Value("ComputeTime [ns]", refSimulation->getComputeTime(), "%.3f");
}

void SPHsimulationUI::frameRateTextInfo()
{
	float fr = 1.0f / (static_cast<float>(refSimulation->getComputeTime())/1000000000.000f);

	ImGui::Value("FrameRate", fr, "%.001f");
}

void SPHsimulationUI::timeStepInput()
{
	float PrevValue = refSimulation->getSimulationStep();
	uiValues.timeStep = PrevValue;
	ImGui::InputFloat("TimeStep", &uiValues.timeStep, 0.0001f, 0.1f, "%.4f");

	if (uiValues.timeStep != PrevValue)
	{
		refSimulation->setSimulationStep(uiValues.timeStep);
	}
}

void SPHsimulationUI::startSimulationButton()
{

	if (*simulationContext != ContextState::RUNNING)
	{
		if (ImGui::Button("StartSimulation"))
		{
			refSimulation->setMemoryLayout();
			*simulationContext = ContextState::RUNNING;
		}	
	}
	else
	{
		if (ImGui::Button("StopSimulation"))
		{
			refSimulation->setMemoryLayout();
			*simulationContext = ContextState::SETUP;
		}
	}

}

void SPHsimulationUI::restartSimulationButton()
{
	if (ImGui::Button("SetSimulation"))
	{
		refSimulation->clearSimulation();
		refSimulation->setSimulationState();
		refSimulation->setFluidAndParticles();
		refSimulation->setMemoryLayout();
	}
}

void SPHsimulationUI::stopSimulationButton()
{
}

void SPHsimulationUI::pauseSimulationButton()
{
}

void SPHsimulationUI::returnToMenuButton()
{
	if (ImGui::Button("BackToMenu"))
	{
		*m_state = ProgramState::MAIN_MENU;
	}
}
