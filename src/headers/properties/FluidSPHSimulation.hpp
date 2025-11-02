#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>
#include <shaderProgram.hpp>
#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>
struct alignas(16) Particle
{
    glm::vec4 position = {0,0,0,0};
    glm::vec4 velocity = {0,0,0,0};
    glm::vec4 acceleration = {0,0,0,0};
};

struct alignas(16) FluidParticle : Particle
{
    float pressure=0;
    float density=0;
    float mass=0;
    float pad0=0;
};


/// @brief cell defined with a position of it's back, lower, left vertex and size right up and front
struct Domain
{
    glm::vec3 posittion = {-2.5,-0.5,0.0};
    glm::vec3 size = {5,5,1}; // {m,m,m}
};

struct Fluid
{
    float fluidDensity = 997; // kg
    float volume = 10.0f; // m^3
    float stiffnesK = 35.0f; // nazewnictwo luźno powiązane z prędkością dźwięku 
    float stiffnesY=7.0f;
};


enum SimDim{
    DIMENSION_1 = 0u,
    DIMENSION_2 = 1u,
    DIMENSION_3 = 2u
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
    FluidSPHSimulation()
    {
        m_pressureNdensityComputeShader = std::make_unique<ShaderProgram>();
        m_pressureNdensityComputeShader->addShader(GL_COMPUTE_SHADER, "shaders/FluidSPH/densityNPressure.shader");
        m_pressureNdensityComputeShader->linkProgram();
        m_movementComputeShader = std::make_unique<ShaderProgram>();
        m_movementComputeShader->addShader(GL_COMPUTE_SHADER, "shaders/FluidSPH/movement.shader");
        m_movementComputeShader->linkProgram();
        m_accelerationComputeShader = std::make_unique<ShaderProgram>();
        m_accelerationComputeShader->addShader(GL_COMPUTE_SHADER, "shaders/FluidSPH/acceleration.shader");
        m_accelerationComputeShader->linkProgram();
    }
    FluidSPHSimulation(std::unique_ptr<ShaderProgram> computePressureShader, std::unique_ptr<ShaderProgram> computeMovementShader, Domain simulationDomain, Fluid fluid, uint32_t numOfParticles);

    void setFluidAndParticles();
    const Fluid& getFluid() const;
    //const Domain& getInitialDomain() const;
    //const Domain& getSimulationDomain() const;
    uint32_t getNumOfParticles() const;
    const std::vector<FluidParticle>& getParticles() const;
    float getParticleRadius(){return particleRadius;}
    void simulationStep(float timeStep);
    GLuint getParticleBuffer() const {return partBuf;}
private:
    GLuint partBuf;
    GLuint FluidBuf;
    std::unique_ptr<ShaderProgram> m_movementComputeShader;
    std::unique_ptr<ShaderProgram> m_accelerationComputeShader;
    std::unique_ptr<ShaderProgram> m_pressureNdensityComputeShader;
    Domain m_simulationDomain, m_initialDomain;
    Fluid m_fluid;
    std::vector<FluidParticle> m_particles;
    uint32_t m_numOfParticles = 25000;
    float m_kernelRadius = 0.1f;
    float particleRadius = 0.0f;
};



