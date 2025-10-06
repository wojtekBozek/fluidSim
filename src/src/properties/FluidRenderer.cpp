#include "FluidRenderer.hpp"

void SPHSimulationRenderer::render()
{
    //computeShaderProgram->useProgram();
    //glDispatchCompute(particlesNumber/1000, 1, 1);
    //glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    shaderProgram->useProgram();
    glm::mat4 viewMatrix = camera->getView();
    glm::mat4 projMatrix = camera->getProjection();
    shaderProgram->setMat4("view", viewMatrix);
    shaderProgram->setMat4("proj", projMatrix);
    shaderProgram->setFloat("viewportHeight", camera->getWindowHeight());
    shaderProgram->setFloat("fovy", camera->getFOV()/180.0f*M_PI);
    //shaderProgram->setMat4("viewMatrix", viewMatrix);
    //shaderProgram->setVec2("halfSize", glm::vec2(simulation.getParticleDiamaeter(),simulation.getParticleDiamaeter()));
    shaderProgram->setVec3("color", glm::vec3(1.0, 0.3, 0.3));
    shaderProgram->setFloat("particleRadius", simulation.getParticleRadius());
    shaderProgram->setFloat("softness", 0.5f);
    glBindVertexArray(quadVAO);
    glDrawArraysInstanced(GL_POINTS, 0, 1, simulation.getNumOfParticles());
    glBindVertexArray(0);
}

void SPHSimulationRenderer::setupBackend()
{
    GLuint bufsize = simulation.getNumOfParticles() * 4 * sizeof(FluidParticle);
    glGenBuffers(1, &posBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, simulation.getParticles().data(), GL_DYNAMIC_DRAW);

    
    //glGenBuffers(1, &velBuf);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velBuf);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, initVel.data(), GL_DYNAMIC_DRAW);

    GLfloat quadVerts[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    //glGenBuffers(1, &quadVBO);
    //glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
    glEnable(GL_PROGRAM_POINT_SIZE);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
//
    //glBindVertexArray(0);

    simulation.setFluidAndParticles();
}
