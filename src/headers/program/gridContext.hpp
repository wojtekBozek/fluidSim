#pragma once
#include "context.hpp"

class GridSimulationContext : public AbstractContext
{
public:
	GridSimulationContext(GLFWwindow* window, std::shared_ptr<ProgramState> state) : AbstractContext(window, state) {};
	void processContext(std::shared_ptr<rendering::Camera> camera) override;
	void initContext() override;
	void showUI() override;
private:
};