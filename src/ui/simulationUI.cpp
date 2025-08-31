#include "simulationUI.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <nfd.h>
#include <string>


SimulationUI::SimulationUI(GLFWwindow *window, std::shared_ptr<ProgramState> state) : m_window(window), m_state(state) {}

void SimulationUI::showUI()
{
    if(*m_state == ProgramState::SIMULATION)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Simulation settings");
        backToMenuButton();
        openDialogButton();
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

void SimulationUI::backToMenuButton()
{
    if(ImGui::Button("Back To Menu"))
    {
        *m_state = ProgramState::MAIN_MENU;
    }
}

void SimulationUI::openDialogButton()
{
    NFD_Init();
    if (ImGui::Button("Open File Dialog"))
    {
        nfdu8char_t *outPath;
        nfdu8filteritem_t filters[2] = { { "Source code", "c,cpp,cc" }, { "Headers", "h,hpp" } };
        nfdopendialogu8args_t args = {0};
        args.filterList = filters;
        args.filterCount = 2;
        nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
        if (result == NFD_OKAY)
        {
            puts("Success!");
            puts(outPath);
            NFD_FreePathU8(outPath);
        }
        else if (result == NFD_CANCEL)
        {
            puts("User pressed cancel.");
        }
        else 
        {
            printf("Error: %s\n", NFD_GetError());
        }
    }
    NFD_Quit();
}
