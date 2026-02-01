#pragma once
#include "context.hpp"
#include "baseGrid.hpp"
#include "mixedSimUI.hpp"
#include "mixedRenderer.hpp"
#include "mixed.hpp"
class MixedSimulationContext : public AbstractContext
{
public:
	MixedSimulationContext(GLFWwindow* window, std::shared_ptr<ProgramState> state) : AbstractContext(window, state) {};
	void processContext(std::shared_ptr<rendering::Camera> camera) override;
	void initContext() override;
	void showUI() override;
private:
	std::shared_ptr<BaseUI> simulationUI;
	std::shared_ptr<ParticleInCell2D> simulation;
	std::shared_ptr<MixedRenderer> renderer;
	std::shared_ptr<ContextState> state;
};