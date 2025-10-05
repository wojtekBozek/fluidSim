#pragma once
#include "FluidSPHSimulation.hpp"
#include "renderer.hpp"

class SPHSimulationRenderer : public BaseRenderer
{
    public:
    SPHSimulationRenderer() = default;
    void render() override;
    void setupBackend();
    void setShaderProgram(std::shared_ptr<ShaderProgram> shader)
    {
        shaderProgram = shader;
    }
    void setCamera(std::shared_ptr<rendering::PerspectiveCamera> cam)
    {
        camera = cam;
    }
    private:
    std::shared_ptr<ShaderProgram> shaderProgram;
    std::shared_ptr<rendering::PerspectiveCamera> camera;
    FluidSPHSimulation simulation;
    GLuint posBuf;
    GLuint quadVAO = 0;
    GLuint quadVBO = 0; 
}; 