#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>

struct Particle
{
    glm::vec3 position;
    glm::vec3 speed;
    glm::vec3 acceleration;
};

struct FluidParticle : Particle
{
    float pressure;
    float density;
    float mass;
};



class FluidParticleManager
{
    public:
    void SimulationStepOnCPU();
    void loadParticlesToGpu();
    private:
    std::vector<FluidParticle> m_fluidParticles;
    float kernelRadius;
    GLuint m_SSBO;
}; 