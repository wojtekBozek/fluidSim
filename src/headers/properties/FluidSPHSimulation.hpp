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

enum particleType : GLuint
{
    FLUID = 0,
    BOUNDARY = 1 
};

enum BoundaryConditions : GLuint
{
    SolidWall = 0,
    ParticleWall = 1,
    GhostParticles = 2
};

struct alignas(16) FluidParticle : Particle
{
    float pressure=0;
    float density=0;
    float mass=0;
    GLuint type = particleType::FLUID;
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
    float stiffnesK = 3.5f;  
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
        m_boundaryParticleMassComputeShader = std::make_unique<ShaderProgram>();
        m_boundaryParticleMassComputeShader->addShader(GL_COMPUTE_SHADER, "shaders/FluidSPH/boundaryParticlesDensity.shader");
        m_boundaryParticleMassComputeShader->linkProgram();

        setInitialState();
    }
    FluidSPHSimulation(std::unique_ptr<ShaderProgram> computePressureShader, std::unique_ptr<ShaderProgram> computeMovementShader, Domain simulationDomain, Fluid fluid, uint32_t numOfParticles);

    void setInitialState();
    void setInitialSimulationDomain();
    void setSimulationState();

    void setFluidAndParticles();
    void setMemoryLayout();
    void setParticleBufferData();

    const Fluid& getFluid() const{ return m_fluid; }
    void setFluidDensity(float density) { m_fluid.fluidDensity = density; }
    void setFluidStiffnessKCoefficient(float k) { m_fluid.stiffnesK = k; }
    void setFluidStiffnessYCoefficient(float y) { m_fluid.stiffnesY = y; }

    uint32_t getNumOfParticles() const { return m_numOfParticles; }

    const std::vector<FluidParticle>& getParticles() const { return m_particles; }

    float getParticleRadius() const {return m_particleRadius;}
    void setParticleRadius(float radius) { m_particleRadius = radius; }

    float getKernelRadius() const { return m_kernelRadius; }
    void setKernelRadius(float radius) { m_kernelRadius = radius; }

    void simulationStep(float timeStep);
    float getSimulationStep() const { return m_timeStep; }
    void setSimulationStep(float timeStep) { m_timeStep = timeStep; }

    const Domain& getSimulationDomain() const { return m_simulationDomain; }
    void setSimuilationDomainSize(glm::vec3 sizeVec) { m_simulationDomain.size = sizeVec; }
    void setSimulationsDomainPosition(glm::vec3 position) { m_simulationDomain.posittion = position; }

    const Domain& getFluidDomain() const { return m_initialDomain; }
    void setFluidDomainSize(glm::vec3 sizeVec) 
    { 
        m_initialDomain.size = sizeVec;
        if (m_dimension == SimDim::DIMENSION_3)
        {
            m_fluid.volume = m_initialDomain.size.x * m_initialDomain.size.y * m_initialDomain.size.z;
            //m_numOfParticles = m_fluid.volume / (1.33 * std::pow(m_particleRadius, 3) * M_PI);
        }
        else if (m_dimension == SimDim::DIMENSION_2)
        {
            m_fluid.volume = m_initialDomain.size.x * m_initialDomain.size.y;// *m_initialDomain.size.z;
            //m_numOfParticles = m_fluid.volume / (std::pow(m_particleRadius, 2) * M_PI);
        }
    }
    void setFluidDomainPosition(glm::vec3 position) { m_initialDomain.posittion = position; }

    GLuint64 getComputeTime() const { return m_computeTime; }
    GLuint getParticleBuffer() const {return m_partBuf;}

    GLuint getSimulationDimension() const { return m_dimension; }
    void setSimulationDimension(float dimension) { m_dimension = dimension; }
    void clearSimulation();
private:
    GLuint m_partBuf;
    GLuint m_fluidBuf;
    GLuint m_hashBuf;
    GLuint m_nextNodeBuf;

    GLuint m_dimension = SimDim::DIMENSION_3;

    std::vector<GLint> m_hashValues;
    std::vector<GLint> m_nextNodes;
    std::unique_ptr<ShaderProgram> m_initHashTableComputeShader;
    std::unique_ptr<ShaderProgram> m_resetHashTableComputeShader;
    std::unique_ptr<ShaderProgram> m_movementComputeShader;
    std::unique_ptr<ShaderProgram> m_accelerationComputeShader;
    std::unique_ptr<ShaderProgram> m_pressureNdensityComputeShader;
    std::unique_ptr<ShaderProgram> m_boundaryParticleMassComputeShader;
    Domain m_simulationDomain, m_initialDomain;
    Fluid m_fluid, refFluid;
    std::vector<FluidParticle> m_particles;
    std::vector<FluidParticle> m_boundaryParticles;
    GLuint hashCoef = 1;
    uint32_t m_numOfParticles = 0;
    float m_timeStep = 0.001f;
    float m_particleRadius = 0.05f;
    float m_kernelCof = 2.5;
    float m_boundCof = 1.0;
    float m_kernelRadius = 0.2f;
    bool m_partBUfferCreated = false;
    GLuint m_particleBufferSize = 0;
    GLuint64 m_computeTime = 0;
    GLuint invocations = 128;
};



