#include "FluidRenderer.hpp"

#include <glm/gtc/type_ptr.hpp>
void SPHSimulationRenderer::render()
{
    //m_computeShader->useProgram();
    //glDispatchCompute(simulation.getNumOfParticles()/3, 1, 1);
    //glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBuf);
    shaderProgram->useProgram();
    glm::mat4 viewMatrix = camera->getView();
    glm::mat4 projMatrix = camera->getProjection();
    shaderProgram->setMat4("view", viewMatrix);
    shaderProgram->setMat4("proj", projMatrix);
    shaderProgram->setFloat("viewportHeight", camera->getWindowHeight());
    shaderProgram->setFloat("fovy", camera->getFOV()/180.0f*M_PI); 
    shaderProgram->setVec3("color", glm::vec3(0.0, 0.3, 0.8));
    shaderProgram->setFloat("particleRadius", simulation.getParticleRadius());
    glBindVertexArray(quadVAO);
    glDrawArraysInstanced(GL_POINTS, 0, 1, simulation.getNumOfParticles());
    glBindVertexArray(0);
}

void SPHSimulationRenderer::setupBackend()
{
    
    //m_computeShader = std::make_unique<ShaderProgram>();
    //m_computeShader->addShader(GL_COMPUTE_SHADER, "shaders/FluidSPH/compute.shader");
    //m_computeShader->linkProgram();
    simulation.setFluidAndParticles();
    GLuint bufsize = simulation.getNumOfParticles()*sizeof(FluidParticle);
    glGenBuffers(1, &posBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, simulation.getParticles().data(), GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    glEnable(GL_PROGRAM_POINT_SIZE);
}
