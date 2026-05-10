#pragma once
#include "programState.hpp"
#include "baseUI.hpp"
#include <memory>
#include "context.hpp"
#include <GLFW/glfw3.h>
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
    void chooseGRID3dSimButton();
    
    void chooseMixedSimButton();
    void exitButton();
    MainMenuState m_menuState;
};