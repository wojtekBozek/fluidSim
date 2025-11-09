#pragma once
#include "baseUI.hpp"
#include "FluidSPHSimulation.hpp"


class SPHsimulationUI : public BaseUI
{
public:
	SPHsimulationUI() = default;
	void showUI() override;
private:

	void fluidStiffKcoeffSlider();
	void fluidStiffYcoeffSlider();
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

	void startSimulationButton();
	void returnToMenuButton();
	std::shared_ptr<FluidSPHSimulation> refSimulation;
};