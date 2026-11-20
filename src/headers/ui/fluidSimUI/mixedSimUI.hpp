#pragma once
#include "baseUI.hpp"
#include "baseGrid.hpp"
#include "mixed.hpp"
#include "mixedRenderer.hpp"
#include "contextState.hpp"
class MixedSimulationUI : public BaseUI
{
public:
	MixedSimulationUI(std::shared_ptr<ContextState> context, std::shared_ptr<MixedRenderer> renderer, std::shared_ptr<ParticleInCell2D> simulation, std::shared_ptr<ProgramState> state) : BaseUI(state)
	{
		refContext = context;
		refRenderer = renderer;
		refSimulation = simulation;
	}
	void show() override;
private:
	void setTimeStep();
	void setSimDim();
	void setFluidDim();
	void setFluidPos();
	void setPressureIterations();
	void setCellSize();
	void setBorderSize();
	void displayCurrentStep();
	void setPicFlipRatio();
	
	void setDensity();
	void setSolver();
	void setPressurePolicy();
	void setOverrelaxation();
	void setNumOfParticlesPerCell();

	void confirmChanges();

	void returnToMenuButton();
	void startSimulation();
	void vVelocityShader();
	void uVelocityShader();
	void divergenceShader();
	void pressureShader();
	void visualShader();
	void particleShader();
	void timeStepTime();
	void frameRate();
	void resetSimulation();
	std::shared_ptr<ParticleInCell2D> refSimulation;
	std::shared_ptr<MixedRenderer> refRenderer;
	std::shared_ptr<ContextState> refContext;
};