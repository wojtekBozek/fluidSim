#pragma once
#include "programState.hpp"
#include "baseUI.hpp"
#include <memory>
#include "context.hpp"
#include <glfw/glfw3.h>
class MainWindow : public AbstractContext
{
public:
    MainWindow(GLFWwindow *window, std::shared_ptr<ProgramState> state);
    void showUI() override;
    void initContext() override;
    void processContext(std::shared_ptr<rendering::Camera> camera) override;
private:    
    void chooseSPHSimButton();
    void chooseGRIDSimButton();
    void textureTestButton();
    void exitButton();
    //std::shared_ptr<ProgramState> m_state;
    //GLFWwindow *m_window;
    MainMenuState m_menuState;
};