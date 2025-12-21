#pragma once
#include "baseUI.hpp"
#include "baseGrid.hpp"
#include "grid2d.hpp"
class GridSimulationUI : public BaseUI
{
public:
	//SPHsimulationUI() = default;
	GridSimulationUI(std::shared_ptr<BaseGrid> simulation, GLFWwindow* window, std::shared_ptr<ProgramState> state) : BaseUI(window, state)
	{
		refSimulation = simulation;
	}
	void showUI() override;
private:
	void returnToMenuButton();
	std::shared_ptr<BaseGrid> refSimulation;
};