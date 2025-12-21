#pragma once
#include "context.hpp"
#include "baseGrid.hpp"
#include "gridSimUI.hpp"
#include "gridrenderer.hpp"
#include "grid2d.hpp"
class GridSimulationContext : public AbstractContext
{
public:
	GridSimulationContext(GLFWwindow* window, std::shared_ptr<ProgramState> state) : AbstractContext(window, state) {};
	void processContext(std::shared_ptr<rendering::Camera> camera) override;
	void initContext() override;
	void showUI() override;
private:
	std::shared_ptr<BaseUI> simulationUI;
	std::shared_ptr<Grid2D> simulation;
	std::shared_ptr<GridRenderer> renderer;
};