#include "particleRenderer.hpp"

void ParticleRenderer::setupBackend()
{
    GLuint bufsize = particlesNumber * 4 * sizeof(GLfloat);
    
    glGenBuffers(1, &posBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, initPos.data(), GL_DYNAMIC_DRAW);

    
    glGenBuffers(1, &velBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, initVel.data(), GL_DYNAMIC_DRAW);

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

void ParticleRenderer::render()
{
    computeShaderProgram->useProgram();
    glDispatchCompute(particlesNumber/1000, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    shaderProgram->useProgram();
    glBindVertexArray(quadVAO);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, particlesNumber);
    glBindVertexArray(0);
}
void ParticleRenderer::generateParticles(GLuint number, float minCoord, float maxCoord)
{
    particlesNumber = number;
    for (int i = 0; i < particlesNumber; ++i)
    {
        float x = minCoord + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxCoord - minCoord)));
        float y = minCoord + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxCoord - minCoord)));
        float z = minCoord + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxCoord - minCoord)));
        initPos.push_back(x);
        initPos.push_back(y);
        initPos.push_back(z);
        initPos.push_back(1.0f);

        float vx = -0.1f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.2f)));
        float vy = -0.1f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.2f)));
        float vz = -0.1f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.2f)));
        initVel.push_back(vx);
        initVel.push_back(vy);
        initVel.push_back(vz);
        initVel.push_back(0.0f);
    }
}
