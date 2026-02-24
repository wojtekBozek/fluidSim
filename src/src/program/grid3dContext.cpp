#include "grid3dContext.hpp"
void Grid3DSimulationContext::processContext(std::shared_ptr<rendering::Camera> camera)
{
	if(ContextState::RUNNING == *state)
		simulation->run();
	renderer->render(camera);
}

void Grid3DSimulationContext::initContext()
{
	simulation = std::make_shared<Grid3D>();
	simulation->setup();
	renderer = std::make_shared<Grid3dRenderer>();
	renderer->setupSimulation(simulation);
	renderer->setupBackend();
	state = std::make_shared<ContextState>(ContextState::SETUP);
	simulationUI = std::make_shared<Grid3dSimulationUI>(Grid3dSimulationUI(state, renderer, simulation, m_window, m_state));
}

void Grid3DSimulationContext::showUI()
{
	simulationUI->showUI();
}
