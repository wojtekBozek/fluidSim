#pragma once
#include <memory>
#include "FluidSPHSimulation.hpp"
#include "FluidRenderer.hpp"
#include "sphSimulationUI.hpp"


class AbstractContext
{
public:
	virtual void initContext() = 0;
	virtual void processContext() = 0;
};

class SPHSimulationContext : public AbstractContext
{
public:
	explicit SPHSimulationContext() = default;
	void processContext() override;
	void initContext() override;

private:
	enum SimulationUIs 
	{
		SETUP_UI,
		RUNNING_UI
	};

	FluidSPHSimulation sphSimulation;
	SPHSimulationRenderer renderer;
	std::shared_ptr<BaseUI> simulationUI;
};