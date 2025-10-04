#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>
#include <shaderProgram.hpp>
#include <memory>
struct Particle
{
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
};

struct FluidParticle : Particle
{
    float pressure;
    float density;
    float mass;
};


/// @brief cell defined with a position of it's back, lower, left vertex and size right up and front
struct Domain
{
    glm::vec3 posittion = {0,0,0};
    glm::vec3 size = {10,10,10}; // {m,m,m}
};

struct Fluid
{
    float fluidDensity = 997; // kg
    float volume = 1000; // m^3
};


class FluidSPHSimulation
{
public:
    FluidSPHSimulation() = default;
    FluidSPHSimulation(std::unique_ptr<ShaderProgram> computeShader, Domain simulationDomain, Fluid fluid, uint32_t numOfParticles);

    void setFluidAndParticles();
private:
    std::unique_ptr<ShaderProgram> m_computeShader;
    Domain m_simulationDomain, m_initialDomain;
    Fluid m_fluid;
    std::vector<FluidParticle> m_particles;
    uint32_t m_numOfParticles;
    float m_kernelRadius;
};



class FluidParticleManager
{
    public:
    private:
    std::vector<FluidParticle> m_fluidParticles;
    float kernelRadius;
    GLuint m_SSBO;
}; 