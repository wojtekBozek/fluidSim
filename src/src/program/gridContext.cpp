#include "gridContext.hpp"

void GridSimulationContext::processContext(std::shared_ptr<rendering::Camera> camera)
{
}

void GridSimulationContext::initContext()
{
	simulationUI = std::make_shared<GridSimulationUI>(GridSimulationUI(simulation, m_window, m_state));
}

void GridSimulationContext::showUI()
{
	simulationUI->showUI();
}
