#include "mixedContext.hpp"
void MixedSimulationContext::processContext(std::shared_ptr<rendering::Camera> camera)
{
	if(ContextState::RUNNING == *state)
		simulation->run();
	renderer->render(camera);
}

void MixedSimulationContext::initContext()
{
	simulation = std::make_shared<ParticleInCell2D>();
	renderer = std::make_shared<MixedRenderer>();
	state = std::make_shared<ContextState>(ContextState::SETUP());
	simulationUI = std::make_shared<MixedSimulationUI>(MixedSimulationUI(state, renderer, simulation, m_window, m_state));
	renderer->setupBackend();
	simulation->setup();
	renderer->setupSimulation(simulation);
}

void MixedSimulationContext::showUI()
{
	simulationUI->showUI();
}
