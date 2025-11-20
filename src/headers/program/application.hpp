#pragma once
#include <memory>
#include <vector>
#include "renderer.hpp"
#include "shaderProgram.hpp"
#include "objectsMeneger.hpp"
#include "setup.hpp"
#include "sceneLoader.hpp"
#include "programState.hpp"
#include "baseUI.hpp"
#include "camera3D.hpp"
#include "positionedLight.hpp"
#include "particleRenderer.hpp"
#include "FluidRenderer.hpp"
#include "context.hpp"
#include "mainWindow.hpp"
class Application
{
public:
    Application() = default;
    virtual ~Application() = default;

    virtual void run() = 0;
};

class MyApp : public Application
{
public:
    MyApp() = default;
    virtual ~MyApp() = default;

    virtual void run() override;
    
private:
    void initialize();
    void mainLoop();
    void cleanup();
    void setupResources();

    std::shared_ptr<ProgramState> programState;
    std::vector<std::shared_ptr<BaseUI>> UIs;
    std::unordered_map<std::string, std::shared_ptr<rendering::Camera>> cameras;
    std::shared_ptr<AbstractContext> activeContext;
    std::unordered_map<ProgramState, std::shared_ptr<AbstractContext>> contextMap;

    double currentFrame = 0.0f;
    double lastFrame = 0.0f;
    GLFWwindow* window = nullptr;
};