#pragma once
#include "context.hpp"
#include "baseGrid.hpp"
#include "gridSim3dUI.hpp"
#include "grid3dRenderer.hpp"
#include "grid3d.hpp"
class Grid3DSimulationContext : public AbstractContext
{
public:
	Grid3DSimulationContext(GLFWwindow* window, std::shared_ptr<ProgramState> state) : AbstractContext(window, state) {};
	void processContext(std::shared_ptr<rendering::Camera> camera) override;
	void initContext() override;
	void showUI() override;
private:
	std::shared_ptr<BaseUI> simulationUI;
	std::shared_ptr<Grid3D> simulation;
	std::shared_ptr<Grid3dRenderer> renderer;
	std::shared_ptr<ContextState> state;
};