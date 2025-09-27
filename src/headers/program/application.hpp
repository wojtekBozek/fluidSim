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


    std::unique_ptr<Renderer> renderer;
    std::shared_ptr<ShaderProgram> shaderProgram;
    std::shared_ptr<ObjectMenager> objectsMenager;
    std::shared_ptr<ProgramState> programState;
    std::vector<std::shared_ptr<BaseUI>> UIs;
    std::unique_ptr<rendering::PerspectiveCamera> camera;
    std::vector<std::shared_ptr<Mesh>> meshes;

    PositionedLight light;


    float currentFrame = 0.0f;
    float lastFrame = 0.0f;
    GLFWwindow* window = nullptr;
};