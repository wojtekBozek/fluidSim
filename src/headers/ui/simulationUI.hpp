#pragma once
#include "baseUI.hpp"
#include "programState.hpp"
#include <GLFW/glfw3.h>
#include <memory>

class SimulationUI : public BaseUI
{
public:
    SimulationUI(std::shared_ptr<ProgramState> state);
    void show() override;
private:
    void backToMenuButton();
    void openDialogButton();
    MainMenuState m_menu_state;

};