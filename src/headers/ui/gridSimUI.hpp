#pragma once
#include "baseUI.hpp"
#include "baseGrid.hpp"
#include "grid2d.hpp"
#include "gridrenderer.hpp"
#include "SPHContextState.hpp"
class GridSimulationUI : public BaseUI
{
public:
	//SPHsimulationUI() = default;
	GridSimulationUI(std::shared_ptr<ContextState> context, std::shared_ptr<GridRenderer> renderer, std::shared_ptr<Grid2D> simulation, GLFWwindow* window, std::shared_ptr<ProgramState> state) : BaseUI(window, state)
	{
		refContext = context;
		refRenderer = renderer;
		refSimulation = simulation;
	}
	void showUI() override;
private:
	void setTimeStep();
	void setSimDim();
	void setFluidDim();
	void setFluidPos();
	void setPressureIterations();
	void setCellSize();
	void setBorderSize();
	void displayCurrentStep();
	
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
	std::shared_ptr<Grid2D> refSimulation;
	std::shared_ptr<GridRenderer> refRenderer;
	std::shared_ptr<ContextState> refContext;
};