#pragma once
#include "baseUI.hpp"
#include "FluidSPHSimulation.hpp"
#include "SPHContextState.hpp"

class SPHsimulationUI : public BaseUI
{
public:
	//SPHsimulationUI() = default;
	SPHsimulationUI(std::shared_ptr<FluidSPHSimulation> simulation, std::shared_ptr<ContextState> context, GLFWwindow* window, std::shared_ptr<ProgramState> state) : BaseUI(window, state)
	{ refSimulation = simulation; simulationContext = context; }
	void showUI() override;
private:
	enum SimulationState
	{
		InitSim,
		processSim,
		pauseSim,
		restartSim
	};
	void fluidStiffKcoeffTextInput();
	void fluidStiffYcoeffTextInput();
	void fluidDensityTextInput();
	void dimensionComboBox();
	void fluidVolumeTextInputs();
	void fluidPositionTextInputs();
	void simulationVolumeTextInputs();
	void simulationPositionTextInputs();
	void sphKernelRadiusSizeInput();
	void visualRadiusSizeInput();

	void numOfParticlesTextInfo();
	void volumeTextInfo();
	void nsComputationTextInfo();
	void frameRateTextInfo();

	void timeStepInput();

	void startSimulationButton();
	void restartSimulationButton();
	void stopSimulationButton();
	void pauseSimulationButton();

	void returnToMenuButton();
	std::shared_ptr<FluidSPHSimulation> refSimulation;
	std::shared_ptr<ContextState> simulationContext;
	struct UiValues
	{
		float k, y, timeStep, density, kernelRadius, visualRadius;
		glm::vec3 simDomainSize, simDomainPos, fluidInitSize, fluidInitPos;
		SimDim dimension;
	}uiValues;
};