#pragma once
#include "FluidSPHSimulation.hpp"
#include "renderer.hpp"

class SPHSimulationRenderer : public BaseRenderer
{
    public:
    SPHSimulationRenderer() = default;
    void render(std::shared_ptr<rendering::Camera> camera) override;
    void setupBackend();
    void setShaderProgram(std::shared_ptr<ShaderProgram> shader)
    {
        shaderProgram = shader;
    }
    void setTimeStep(float timeStep){m_timeStep = timeStep;}
    private:
    float m_timeStep;
    std::unique_ptr<ShaderProgram> m_computeShader;
    std::shared_ptr<ShaderProgram> shaderProgram;
    FluidSPHSimulation simulation;
    GLuint posBuf;
    GLuint quadVAO = 0;
    GLuint quadVBO = 0; 
}; 