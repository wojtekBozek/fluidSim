#pragma once
#include <memory>
#include "FluidSPHSimulation.hpp"
#include "FluidRenderer.hpp"
#include "sphSimulationUI.hpp"
#include "camera3D.hpp"
#include "contextState.hpp"
class AbstractContext
{
public:
	AbstractContext(GLFWwindow* window, std::shared_ptr<ProgramState> state) : m_state(state){};
	virtual void initContext() = 0;
	virtual void processContext(std::shared_ptr<rendering::Camera> camera) = 0;
	void toogleUI(){
		if(ui) {
			ui.get()->toogle();
		}
	}
	void showUI() const {
		ui->show();
	}
	protected:
	std::shared_ptr<BaseUI> ui;
	std::shared_ptr<ProgramState> m_state;
};

class SPHSimulationContext : public AbstractContext
{
public:
	SPHSimulationContext(GLFWwindow* window, std::shared_ptr<ProgramState> state) : AbstractContext(window, state){}
	void processContext(std::shared_ptr<rendering::Camera> camera) override;
	void initContext() override;

private:
	std::shared_ptr<ContextState> state;
	std::shared_ptr<ShaderProgram> sphShaderProgram;
	std::shared_ptr<FluidSPHSimulation> sphSimulation;
	std::shared_ptr<SPHSimulationRenderer> sphRenderer;
};

