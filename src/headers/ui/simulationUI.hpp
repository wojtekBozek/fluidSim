#pragma once
#include "baseUI.hpp"
#include "programState.hpp"
#include <glfw/glfw3.h>
#include <memory>

class SimulationUI : public BaseUI
{
public:
    SimulationUI(GLFWwindow* window, std::shared_ptr<ProgramState> state);
    void showUI() override;
private:
    void backToMenuButton();
    void openDialogButton();
    std::shared_ptr<ProgramState> m_state;
    MainMenuState m_menu_state;
    GLFWwindow *m_window;

};