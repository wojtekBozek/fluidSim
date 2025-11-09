#include "sphSimulationUI.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


void SPHsimulationUI::showUI()
{
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
	ImGui::InputFloat("StiffnessY", &uiValues.y, 1.0f, 15.0f, "%.1f");

	if (uiValues.y != PrevValue)
	{
		refSimulation->setFluidStiffnessYCoefficient(uiValues.y);
	}
}

void SPHsimulationUI::fluidDensityTextInput()
{
	float PrevValue = refSimulation->getFluid().fluidDensity;
	uiValues.y = PrevValue;
	ImGui::InputFloat("Density", &uiValues.density, 0.0f, 10000000.0f, "%.0001f");

	if (uiValues.density != PrevValue)
	{
		refSimulation->setFluidDensity(uiValues.density);
	}
}

void SPHsimulationUI::dimensionComboBox()
{
	
}

void SPHsimulationUI::fluidVolumeTextInputs()
{
	glm::vec3 PrevValue = refSimulation->getFluidDomain().size;
	uiValues.fluidInitSize = PrevValue;
	ImGui::InputFloat("SimDomainSizeX", &uiValues.fluidInitSize.x, 0.0f, 10000000.0f, "%.0001f");
	ImGui::InputFloat("SimDomainSizeY", &uiValues.fluidInitSize.y, 0.0f, 10000000.0f, "%.0001f");
	if(refSimulation->getSimulationDimension() == SimDim::DIMENSION_3)
		ImGui::InputFloat("SimDomainSizeZ", &uiValues.fluidInitSize.z, 0.0f, 10000000.0f, "%.0001f");

	if (uiValues.fluidInitSize != PrevValue)
	{
		refSimulation->setFluidDomainSize(uiValues.fluidInitSize);
	}
}

void SPHsimulationUI::fluidPositionTextInputs()
{
	glm::vec3 PrevValue = refSimulation->getFluidDomain().posittion;
	uiValues.fluidInitPos = PrevValue;
	ImGui::InputFloat("fluidInitPosX", &uiValues.fluidInitPos.x, -10000000.0f, 10000000.0f, "%.0001f");
	ImGui::InputFloat("fluidInitPosY", &uiValues.fluidInitPos.y, -10000000.0f, 10000000.0f, "%.0001f");
	if (refSimulation->getSimulationDimension() == SimDim::DIMENSION_3)
		ImGui::InputFloat("fluidInitPosZ", &uiValues.fluidInitPos.z, -10000000.0f, 10000000.0f, "%.0001f");

	if (uiValues.fluidInitPos != PrevValue)
	{
		refSimulation->setFluidDomainSize(uiValues.fluidInitPos);
	}
}

void SPHsimulationUI::simulationVolumeTextInputs()
{
	glm::vec3 PrevValue = refSimulation->getSimulationDomain().size;
	uiValues.simDomainSize = PrevValue;
	ImGui::InputFloat("SimDomainSizeX", &uiValues.simDomainSize.x, 0.0f, 10000000.0f, "%.0001f");
	ImGui::InputFloat("SimDomainSizeY", &uiValues.simDomainSize.y, 0.0f, 10000000.0f, "%.0001f");
	if (refSimulation->getSimulationDimension() == SimDim::DIMENSION_3)
		ImGui::InputFloat("SimDomainSizeZ", &uiValues.simDomainSize.z, 0.0f, 10000000.0f, "%.0001f");

	if (uiValues.simDomainSize != PrevValue)
	{
		refSimulation->setSimuilationDomainSize(uiValues.simDomainSize);
	}
}

void SPHsimulationUI::simulationPositionTextInputs()
{
	glm::vec3 PrevValue = refSimulation->getSimulationDomain().posittion;
	uiValues.simDomainPos = PrevValue;
	ImGui::InputFloat("simDomainPosX", &uiValues.simDomainPos.x, -10000000.0f, 10000000.0f, "%.0001f");
	ImGui::InputFloat("simDomainPosY", &uiValues.simDomainPos.y, -10000000.0f, 10000000.0f, "%.0001f");
	if (refSimulation->getSimulationDimension() == SimDim::DIMENSION_3)
		ImGui::InputFloat("simDomainPosZ", &uiValues.simDomainPos.z, -10000000.0f, 10000000.0f, "%.0001f");
	if (uiValues.simDomainPos != PrevValue)
	{
		refSimulation->setSimulationsDomainPosition(uiValues.simDomainPos);
	}
}

void SPHsimulationUI::sphKernelRadiusSizeInput()
{
	float PrevValue = refSimulation->getKernelRadius();
	uiValues.kernelRadius = PrevValue;
	ImGui::InputFloat("kernelRadius", &uiValues.kernelRadius, 0.0f, 10.0f, "%.01f");

	if (uiValues.kernelRadius != PrevValue)
	{
		refSimulation->setKernelRadius(uiValues.kernelRadius);
	}
}

void SPHsimulationUI::visualRadiusSizeInput()
{
	float PrevValue = refSimulation->getParticleRadius();
	uiValues.visualRadius = PrevValue;
	ImGui::InputFloat("ParticleRadius", &uiValues.visualRadius, 0.0f, 10.0f, "%.01f");

	if (uiValues.visualRadius != PrevValue)
	{
		refSimulation->setParticleRadius(uiValues.visualRadius);
	}
}

void SPHsimulationUI::numOfParticlesTextInfo()
{
}

void SPHsimulationUI::volumeTextInfo()
{
}

void SPHsimulationUI::nsComputationTextInfo()
{
}

void SPHsimulationUI::frameRateTextInfo()
{
}

void SPHsimulationUI::startSimulationButton()
{
}

void SPHsimulationUI::restartSimulationButton()
{
}

void SPHsimulationUI::stopSimulationButton()
{
}

void SPHsimulationUI::pauseSimulationButton()
{
}

void SPHsimulationUI::returnToMenuButton()
{
}
