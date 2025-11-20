#pragma once
#include "programState.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
class BaseUI
{
public:
    BaseUI(GLFWwindow* window, std::shared_ptr<ProgramState> state) : m_window(window), m_state(state){}
    virtual void showUI() = 0;
    virtual ~BaseUI() = default;
protected:
    GLFWwindow* m_window;
    std::shared_ptr<ProgramState> m_state;
};