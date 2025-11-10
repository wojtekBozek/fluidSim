#pragma once
#include <memory>
#include "FluidSPHSimulation.hpp"
#include "FluidRenderer.hpp"
#include "sphSimulationUI.hpp"
#include "camera3D.hpp"
#include "SPHContextState.hpp"
class AbstractContext : public BaseUI
{
public:
	virtual void initContext() = 0;
	virtual void processContext(std::shared_ptr<rendering::Camera> camera) = 0;
};

class SPHSimulationContext : public AbstractContext
{
public:
	SPHSimulationContext() {}
	void processContext(std::shared_ptr<rendering::Camera> camera) override;
	void initContext() override;
	void showUI() override
	{
		simulationUI->showUI();
	}

private:
	std::shared_ptr<ContextState> state;
	std::shared_ptr<ShaderProgram> sphShaderProgram;
	std::shared_ptr<FluidSPHSimulation> sphSimulation;
	std::shared_ptr<SPHSimulationRenderer> sphRenderer;
	std::shared_ptr<BaseUI> simulationUI;
};