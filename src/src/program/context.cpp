#include "context.hpp"

void SPHSimulationContext::processContext(std::shared_ptr<rendering::Camera> camera)
{
	if (ContextState::RUNNING == *state)
	{
		sphSimulation->simulationStep(1);
	}
		 
	sphRenderer->render(camera);
}

void SPHSimulationContext::initContext()
{
	sphSimulation = std::make_shared<FluidSPHSimulation>(FluidSPHSimulation());
	sphSimulation->setFluidAndParticles();
	sphSimulation->setParticleBufferData();
	state = std::make_shared<ContextState>(ContextState::SETUP());
	simulationUI = std::make_shared<SPHsimulationUI>(SPHsimulationUI(sphSimulation, state));

	sphShaderProgram = std::make_shared<ShaderProgram>();
	sphShaderProgram->addShader(GL_VERTEX_SHADER, "shaders/FluidSPH/vertex.shader");
	sphShaderProgram->addShader(GL_FRAGMENT_SHADER, "shaders/FluidSPH/fragment.shader");
	sphShaderProgram->linkProgram();

	sphRenderer = std::make_shared<SPHSimulationRenderer>();
	sphRenderer->setShaderProgram(sphShaderProgram);
	sphRenderer->setSimulation(sphSimulation);
	sphRenderer->setupBackend();
}
