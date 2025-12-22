#include "FluidRenderer.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "perspectiveCamera.hpp"
#include "orthographicCamera.hpp"

void SPHSimulationRenderer::render(std::shared_ptr<rendering::Camera> camera)
{/**/
    //simulation.simulationStep(m_timeStep);
    shaderProgram->useProgram();
    glm::mat4 viewMatrix = camera->getView();
    glm::mat4 projMatrix = camera->getProjection();
    shaderProgram->setMat4("view", viewMatrix);
    shaderProgram->setMat4("proj", projMatrix);
    shaderProgram->setFloat("viewportHeight", camera->getWindowHeight());
    shaderProgram->setUint("numOfParticles", simulation->getNumOfParticles());
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
    shaderProgram->setFloat("particleRadius", simulation->getParticleRadius());
    glBindVertexArray(quadVAO);
    glDrawArraysInstanced(GL_POINTS, 0, 1, simulation->getNumOfParticles());
    glBindVertexArray(0);
}

void SPHSimulationRenderer::setupBackend()
{
    //simulation->setFluidAndParticles();

    GLuint dummyVBO;
    float dummy = 0.0f;

    glGenBuffers(1, &dummyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, dummyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float), &dummy, GL_STATIC_DRAW);

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,              // location
        1,              // size
        GL_FLOAT,
        GL_FALSE,
        sizeof(float),
        (void*)0
    );
    glBindVertexArray(0);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);  // Default: passes if incoming depth < stored depth
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
