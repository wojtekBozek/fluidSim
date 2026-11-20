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
    class InternalUI : public BaseUI {
        public:
        InternalUI(GLFWwindow *window, std::shared_ptr<ProgramState> state) : BaseUI(state), m_window(window) {}
        void show() override;
        private:
        
        GLFWwindow *m_window;
    
        void chooseSPHSimButton();
        void chooseGRIDSimButton();
        void textureTestButton();
        void texture3DTestButton();
        void chooseGRID3dSimButton();
        
        void chooseMixedSimButton();
        void exitButton();
    };
    void initContext() override;
    void processContext(std::shared_ptr<rendering::Camera> camera) override;
private:    
    MainMenuState m_menuState;
};