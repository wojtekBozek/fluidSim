#pragma once
#include <memory>
#include "renderer.hpp"
#include "shaderProgram.hpp"
#include "objectsMeneger.hpp"
#include "setup.hpp"
#include "sceneLoader.hpp"


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

    std::unique_ptr<Renderer> renderer;
    std::shared_ptr<ShaderProgram> shaderProgram;
    std::shared_ptr<ObjectMenager> objectsMenager;
};