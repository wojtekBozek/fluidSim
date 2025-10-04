#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>
#include <shaderProgram.hpp>
#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>
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
    float soundSpeed = 10.0f; // nazewnictwo luźno powiązane z prędkością dźwięku 
};


enum SimDim{
    DIMENSION_1,
    DIMENSION_2,
    DIMENSION_3
};

inline float CubicSplineKernel(SimDim dimension, float kernelRadius, float distance)
{
    float alfa;
    auto calc1d = [kernelRadius](){return 1.0f/kernelRadius;};
    auto calc2d = [kernelRadius](){return 15.0f/(7*M_PI*kernelRadius*kernelRadius);};
    auto calc3d = [kernelRadius](){return 3.0f/(2*M_PI*std::pow(kernelRadius,3));};

    switch (dimension)
    {
        case DIMENSION_1: alfa = calc1d();
        case DIMENSION_2: alfa = calc2d();
        case DIMENSION_3: alfa = calc3d();
        default: alfa = calc3d();
    }

    float q = distance/kernelRadius;
    float retVal = 0.0f;

    if (q >= 0.0f && q <1.0f)
    {
        retVal = 3/2.0f - q*q + 0.5f*std::pow(q,3);
    }
    else if(q >= 1.0f && q<2.0f)
    {
        retVal = 1/6.0f*std::pow((2-q),3);
    } // else 0, obsłużone z defaultu
    return alfa*retVal; 
}

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