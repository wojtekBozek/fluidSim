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
    glm::vec3 posittion = {-20,-2,-10};
    glm::vec3 size = {200,200,200}; // {m,m,m}
};

struct Fluid
{
    float fluidDensity = 1000; // kg
    float volume = 10.0f; // m^3
    float stiffnesK = 3.5f; // nazewnictwo luźno powiązane z prędkością dźwięku 
    float stiffnesY= 5.0f;
};


enum SimDim{
    DIMENSION_1 = 0u,
    DIMENSION_2 = 1u,
    DIMENSION_3 = 2u
};

class FluidSPHSimulation
{
public:
    FluidSPHSimulation()
    {
        m_resetHashTableComputeShader = std::make_unique<ShaderProgram>();
        m_resetHashTableComputeShader->addShader(GL_COMPUTE_SHADER, "shaders/FluidSPH/resetingHash.shader");
        m_resetHashTableComputeShader->linkProgram();
        m_initHashTableComputeShader = std::make_unique<ShaderProgram>();
        m_initHashTableComputeShader->addShader(GL_COMPUTE_SHADER, "shaders/FluidSPH/hashingFunction.shader");
        m_initHashTableComputeShader->linkProgram();
        m_pressureNdensityComputeShader = std::make_unique<ShaderProgram>();
        m_pressureNdensityComputeShader->addShader(GL_COMPUTE_SHADER, "shaders/FluidSPH/densityNpressureHashed.shader");
        m_pressureNdensityComputeShader->linkProgram();
        m_movementComputeShader = std::make_unique<ShaderProgram>();
        m_movementComputeShader->addShader(GL_COMPUTE_SHADER, "shaders/FluidSPH/movement.shader");
        m_movementComputeShader->linkProgram();
        m_accelerationComputeShader = std::make_unique<ShaderProgram>();
        m_accelerationComputeShader->addShader(GL_COMPUTE_SHADER, "shaders/FluidSPH/accelerationHashed.shader");
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
    GLuint hashBuf;
    GLuint nextNodeBuf;

    GLuint dimension = SimDim::DIMENSION_3;

    std::vector<GLint> hashValues;
    std::vector<GLint> nextNodes;
    std::unique_ptr<ShaderProgram> m_initHashTableComputeShader;
    std::unique_ptr<ShaderProgram> m_resetHashTableComputeShader;
    std::unique_ptr<ShaderProgram> m_movementComputeShader;
    std::unique_ptr<ShaderProgram> m_accelerationComputeShader;
    std::unique_ptr<ShaderProgram> m_pressureNdensityComputeShader;
    Domain m_simulationDomain, m_initialDomain;
    Fluid m_fluid;
    std::vector<FluidParticle> m_particles;
    uint32_t m_numOfParticles = 0;
    float particleRadius = 0.05f;
    float kernelCof = 3.5;
    float boundCof = 1.0;
    float m_kernelRadius = 0.2f;
};



