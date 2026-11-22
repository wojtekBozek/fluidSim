#pragma once
#include "programState.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <imgui.h>
class BaseUI
{
public:
    BaseUI(std::shared_ptr<ProgramState> state) : m_state(state){}
    void toogle() {
        flags ^= ImGuiWindowFlags_NoInputs;
        flags ^= ImGuiWindowFlags_NoNav;
    }
    virtual void show() = 0;
    virtual ~BaseUI() = default;
protected:
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove;
    std::shared_ptr<ProgramState> m_state;
};