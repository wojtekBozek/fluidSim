#include "gridContext.hpp"
void GridSimulationContext::processContext(std::shared_ptr<rendering::Camera> camera)
{
	if(ContextState::RUNNING == *state)
		simulation->run();
	renderer->render(camera);
}

void GridSimulationContext::initContext()
{
	simulation = std::make_shared<Grid2D>();
	renderer = std::make_shared<GridRenderer>();
	state = std::make_shared<ContextState>(ContextState::SETUP);
	ui = std::make_shared<GridSimulationUI>(GridSimulationUI(state, renderer, simulation, m_state));
	renderer->setupBackend();
	simulation->setup();
	renderer->setupSimulation(simulation);
}
