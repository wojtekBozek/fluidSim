#include "FluidRenderer.hpp"

void SPHSimulationRenderer::render()
{
    //computeShaderProgram->useProgram();
    //glDispatchCompute(particlesNumber/1000, 1, 1);
    //glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    //shaderProgram->useProgram();
    glm::mat4 VP = camera->getProjection() * camera->getView();
    glm::mat4 viewMatrix = camera->getView();
    shaderProgram->setMat4("viewProj", VP);
    shaderProgram->setMat4("viewMatrix", viewMatrix);
    //GLint loc = glGetUniformLocation(shaderProgram->getProgramId(), "viewProj");
    //glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(VP));
    //GLint loc2 = glGetUniformLocation(shaderProgram->getProgramId(), "viewMatrix");
    //glUniformMatrix4fv(loc2, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    //GLint loc3 = glGetUniformLocation(shaderProgram->getProgramId(), "upView");
    //glUniform3fv(loc3, 1, glm::value_ptr(camUp));
    glBindVertexArray(quadVAO);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, simulation.getNumOfParticles());
    glBindVertexArray(0);
}

void SPHSimulationRenderer::setupBackend()
{
    GLuint bufsize = simulation.getNumOfParticles() * 4 * sizeof(GLfloat);
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

    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    glBindVertexArray(0);
}
