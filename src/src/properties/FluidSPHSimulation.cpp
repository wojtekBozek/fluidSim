#include "FluidSPHSimulation.hpp"
#include <iostream>
FluidSPHSimulation::FluidSPHSimulation(std::unique_ptr<ShaderProgram> computePressureShader, std::unique_ptr<ShaderProgram> computeMovementShader, Domain simulationDomain, Fluid fluid, uint32_t numOfParticles)
: m_pressureNdensityComputeShader(std::move(computePressureShader)), m_movementComputeShader(std::move(computeMovementShader))
{
    m_simulationDomain = simulationDomain;
    m_fluid = fluid;
    m_numOfParticles = numOfParticles;
    m_particles.reserve(numOfParticles);
}

void FluidSPHSimulation::setFluidAndParticles()
{
    FluidParticle initialParticle;
    initialParticle.mass = (m_fluid.fluidDensity * m_fluid.volume) /static_cast<float>(m_numOfParticles); // masa cząstki
    float domainVolume = m_initialDomain.size.x*m_initialDomain.size.y;// 2d*m_initialDomain.size.z;
    float particleDiameter = std::sqrtf(domainVolume/static_cast<float>(m_numOfParticles));
    particleRadius = particleDiameter/2.0f;
    std::cout << particleRadius << ", " << initialParticle.mass << " - particle radius, mass\n";
    initialParticle.position = glm::vec4(m_initialDomain.posittion + glm::vec3(particleRadius), 1.0f);
    initialParticle.velocity = glm::vec4(glm::vec3(0.0f), 1.0f);
    initialParticle.acceleration = glm::vec4(glm::vec3(0.0f), 1.0f);
    uint32_t xMax = std::ceil(m_initialDomain.size.x/particleDiameter)-1;
    uint32_t yMax = std::ceil(m_initialDomain.size.y/particleDiameter)-1;
    uint32_t zMax = 0; //2d = std::ceil(m_initialDomain.size.z/particleDiameter)-1;
    
    uint32_t a=0;
    uint32_t b=0;
    uint32_t c=0;
    m_particles.reserve(m_numOfParticles);
    for(int i=0; i<m_numOfParticles; ++i)
    {
        FluidParticle part = initialParticle;
        part.position += glm::vec4(glm::vec3(a*particleDiameter, b*particleDiameter, c*particleDiameter),0.0f);
        m_particles.push_back(part);
        if(a < xMax)
        {
            a++;
        }
        else
        {
            a = 0;
            if(b < yMax || 0 == zMax)
            {
                b++;
            }
            else
            {
                b = 0;
                c++;    
            }
        }
    }
    GLuint bufsize = m_numOfParticles * sizeof(FluidParticle);
    glGenBuffers(1, &partBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, partBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, m_particles.data(), GL_DYNAMIC_DRAW);
    
    bufsize = sizeof(Fluid);
    glGenBuffers(1, &FluidBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, FluidBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, &m_fluid, GL_DYNAMIC_DRAW);
}

const Fluid &FluidSPHSimulation::getFluid() const
{
    return m_fluid;
}

uint32_t FluidSPHSimulation::getNumOfParticles() const
{
    return m_numOfParticles;
}

const std::vector<FluidParticle> &FluidSPHSimulation::getParticles() const
{
    return m_particles;
}

void FluidSPHSimulation::simulationStep(float timeStep)
{
    GLuint query;
    glGenQueries(1, &query);

    glBeginQuery(GL_TIME_ELAPSED, query);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, partBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, FluidBuf);
    m_pressureNdensityComputeShader->useProgram();
    m_pressureNdensityComputeShader->setUint("numOfParticles", m_numOfParticles);
    m_pressureNdensityComputeShader->setFloat("sphKernelRadius", 7.5 * particleRadius);
    m_pressureNdensityComputeShader->setUint("DIMENSION", SimDim::DIMENSION_2);
    glDispatchCompute((m_numOfParticles + 999) / 1000, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    m_accelerationComputeShader->useProgram();
    m_accelerationComputeShader->setUint("numOfParticles", m_numOfParticles);
    m_accelerationComputeShader->setFloat("sphKernelRadius", 7.5 * particleRadius);
    m_accelerationComputeShader->setUint("DIMENSION", SimDim::DIMENSION_2);
    m_accelerationComputeShader->setVec3("externalAccelerations", {0.0f,-9.8f,0.0f});
    m_accelerationComputeShader->setVec3("domainRefPos", {-5,-2,-1});
    m_accelerationComputeShader->setVec3("domainDimennsions", {10,10,2});
    m_accelerationComputeShader->setFloat("boundaryMaxDist", 15 * particleRadius);
    m_accelerationComputeShader->setUint("toonerP", 7);
    m_accelerationComputeShader->setFloat("stiffnessK", 35000);
    m_accelerationComputeShader->setFloat("epsilon", 0.05f);
    glDispatchCompute((m_numOfParticles + 999) / 1000, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    m_movementComputeShader->useProgram();
    m_movementComputeShader->setUint("numOfParticles", m_numOfParticles);
    m_movementComputeShader->setFloat("timeStep", timeStep);
    glDispatchCompute((m_numOfParticles + 999) / 1000, 1, 1);

    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
    glEndQuery(GL_TIME_ELAPSED);

    GLuint64 elapsedTime = 0;
    glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsedTime);
    std::cout << "Compute time (ns): " << elapsedTime << std::endl;
}
