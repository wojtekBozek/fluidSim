#pragma once
#include "programState.hpp"
#include "baseUI.hpp"
#include <memory>
#include <glfw/glfw3.h>
class MainWindow : public BaseUI
{
public:
    MainWindow(GLFWwindow *window, std::shared_ptr<ProgramState> state);
    void showUI() override;
private:    
    void chooseSimulationButton();
    void exitButton();
    std::shared_ptr<ProgramState> m_state;
    MainMenuState m_menu_state;
    GLFWwindow *m_window;
};