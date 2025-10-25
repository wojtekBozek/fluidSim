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


    std::vector<std::shared_ptr<BaseRenderer>> renderers;
    std::shared_ptr<MeshRenderer> meshRenderer;
    std::shared_ptr<ParticleRenderer> particleRenderer;
    std::shared_ptr<SPHSimulationRenderer> sphRenderer;
    std::shared_ptr<ShaderProgram> shaderProgram;
    std::shared_ptr<ShaderProgram> particleShaderProgram;
    std::shared_ptr<ShaderProgram> sphShaderProgram;
    std::shared_ptr<ShaderProgram> particleComputeShaderProgram;
    std::shared_ptr<ObjectMenager> objectsMenager;
    std::shared_ptr<ProgramState> programState;
    std::vector<std::shared_ptr<BaseUI>> UIs;
    std::shared_ptr<rendering::Camera> camera;
    std::shared_ptr<rendering::Camera> camera2;
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::shared_ptr<PositionedLight> light;


    double currentFrame = 0.0f;
    double lastFrame = 0.0f;
    GLFWwindow* window = nullptr;
};