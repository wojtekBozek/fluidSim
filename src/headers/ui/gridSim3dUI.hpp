#pragma once
#include "baseUI.hpp"
#include "baseGrid.hpp"
#include "grid3d.hpp"
#include "grid3drenderer.hpp"
#include "SPHContextState.hpp"
class Grid3dSimulationUI : public BaseUI
{
public:
	//SPHsimulationUI() = default;
	Grid3dSimulationUI(std::shared_ptr<ContextState> context, std::shared_ptr<Grid3dRenderer> renderer, std::shared_ptr<Grid3D> simulation, GLFWwindow* window, std::shared_ptr<ProgramState> state) : BaseUI(window, state)
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
	std::shared_ptr<Grid3D> refSimulation;
	std::shared_ptr<Grid3dRenderer> refRenderer;
	std::shared_ptr<ContextState> refContext;
};