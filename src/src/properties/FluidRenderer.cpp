#include "FluidRenderer.hpp"

#include <glm/gtc/type_ptr.hpp>
void SPHSimulationRenderer::render()
{
    simulation.simulationStep(m_timeStep);
    shaderProgram->useProgram();
    glm::mat4 viewMatrix = camera->getView();
    glm::mat4 projMatrix = camera->getProjection();
    shaderProgram->setMat4("view", viewMatrix);
    shaderProgram->setMat4("proj", projMatrix);
    shaderProgram->setFloat("viewportHeight", camera->getWindowHeight());
    shaderProgram->setUint("numOfParticles", simulation.getNumOfParticles());
    if(camera->getCameraType() == rendering::CameraType::perspective)
    {
        shaderProgram->setBool("perspectiveProjection", true);
        shaderProgram->setFloat("fovy", static_cast<rendering::PerspectiveCamera*>(camera.get())->getFOV()/180.0f*M_PI); 
    }
    else
    {
        shaderProgram->setBool("perspectiveProjection", false);
        shaderProgram->setFloat("top", static_cast<rendering::OrthographicCamera*>(camera.get())->getTop());
        shaderProgram->setFloat("bottom", static_cast<rendering::OrthographicCamera*>(camera.get())->getBottom());
    }
    shaderProgram->setVec3("color", glm::vec3(0.0, 0.3, 0.8));
    shaderProgram->setFloat("particleRadius", simulation.getParticleRadius());
    glBindVertexArray(quadVAO);
    glDrawArraysInstanced(GL_POINTS, 0, 1, simulation.getNumOfParticles());
    glBindVertexArray(0);
}

void SPHSimulationRenderer::setupBackend()
{
    simulation.setFluidAndParticles();

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    glEnable(GL_PROGRAM_POINT_SIZE);
}
