#include "gridContext.hpp"
void GridSimulationContext::processContext(std::shared_ptr<rendering::Camera> camera)
{
	simulation->run();
	renderer->render(camera);
}

void GridSimulationContext::initContext()
{
	simulation = std::make_shared<Grid2D>();
	simulationUI = std::make_shared<GridSimulationUI>(GridSimulationUI(simulation, m_window, m_state));
	renderer = std::make_shared<GridRenderer>();
	renderer->setupBackend();
	simulation->setup();
	renderer->setupSimulation(simulation);
}

void GridSimulationContext::showUI()
{
	simulationUI->showUI();
}
