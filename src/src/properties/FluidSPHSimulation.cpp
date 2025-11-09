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
    if (m_dimension == SimDim::DIMENSION_3) 
    {
        m_initialDomain.posittion = glm::vec3(-2.5 + 4 * m_particleRadius, 0.0 + 4 * m_particleRadius, -1.0 + 0.4 * m_particleRadius);

        m_initialDomain.size = glm::vec3(5.0, 5.0, 2.5);
        m_simulationDomain.posittion = glm::vec3(-5.0, -2.5, -5.0);

        m_simulationDomain.size = glm::vec3(10.0, 10.0, 10.0);
        m_fluid.volume = m_initialDomain.size.x * m_initialDomain.size.y * m_initialDomain.size.z;
        m_numOfParticles = m_fluid.volume / (1.33 * std::pow(m_particleRadius, 3) * M_PI);
    }
    else if (m_dimension == SimDim::DIMENSION_2)
    {
        m_initialDomain.posittion = glm::vec3(-10.0 + 4 * m_particleRadius, 0.0 + 4 * m_particleRadius, -m_particleRadius);

        m_initialDomain.size = glm::vec3(20.0, 10.0, 2 * m_particleRadius);

        m_fluid.fluidDensity *= m_particleRadius;
        m_fluid.volume = m_initialDomain.size.x * m_initialDomain.size.y;// *m_initialDomain.size.z;
        m_numOfParticles = m_fluid.volume / (std::pow(m_particleRadius, 2) * M_PI);
    }

    m_kernelRadius = m_kernelCof *m_particleRadius;
    FluidParticle initialParticle;
    float particleDiameter = m_particleRadius *2.0f;
    initialParticle.mass = (m_fluid.fluidDensity * m_fluid.volume) /static_cast<float>(m_numOfParticles); // masa cząstki
    std::cout << "Num of particles: " << m_numOfParticles << "\n";
    std::cout << m_particleRadius << ", " << initialParticle.mass << " - particle radius, mass\n";
    initialParticle.position = glm::vec4(m_initialDomain.posittion + glm::vec3(m_particleRadius), 1.0f);
    initialParticle.velocity = glm::vec4(glm::vec3(0.0f), 1.0f);
    initialParticle.acceleration = glm::vec4(glm::vec3(0.0f), 1.0f);
    uint32_t xMax = std::ceil(m_initialDomain.size.x/particleDiameter);
    uint32_t yMax = std::ceil(m_initialDomain.size.y/particleDiameter)-1;
    uint32_t zMax = 0;
    if (m_dimension == SimDim::DIMENSION_3)
       zMax = std::ceil(m_initialDomain.size.z/particleDiameter)-1;
    
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
                if (c == zMax)
                {
                    a = 0;
                }
            }
        }
    }
    GLuint bufsize = m_numOfParticles * sizeof(FluidParticle);
    glGenBuffers(1, &m_partBuf);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_partBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, m_particles.data(), GL_DYNAMIC_DRAW);
    
    bufsize = sizeof(Fluid);
    glGenBuffers(1, &m_fluidBuf);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_fluidBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, &m_fluid, GL_DYNAMIC_DRAW);

    m_hashValues = std::vector<GLint>(m_numOfParticles, -1);
    m_nextNodes = std::vector<GLint>(m_numOfParticles, -1);

    bufsize = m_numOfParticles * sizeof(GLint);
    glGenBuffers(1, &m_hashBuf);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_hashBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, m_hashValues.data(), GL_DYNAMIC_DRAW);

    glGenBuffers(1, &m_nextNodeBuf);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nextNodeBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, m_nextNodes.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


    m_resetHashTableComputeShader->useProgram();
    m_resetHashTableComputeShader->setUint("numOfParticles", m_numOfParticles);
    m_resetHashTableComputeShader->setUint("tableSize", m_numOfParticles);

    m_initHashTableComputeShader->useProgram();
    m_initHashTableComputeShader->setUint("numOfParticles", m_numOfParticles);
    m_initHashTableComputeShader->setUint("tableSize", m_numOfParticles);
    m_initHashTableComputeShader->setFloat("cellSize", 2.0 * m_kernelRadius);

    m_pressureNdensityComputeShader->useProgram();
    m_pressureNdensityComputeShader->setUint("numOfParticles", m_numOfParticles);
    m_pressureNdensityComputeShader->setFloat("sphKernelRadius", m_kernelRadius);
    m_pressureNdensityComputeShader->setUint("tableSize", m_numOfParticles);
    m_pressureNdensityComputeShader->setFloat("cellSize", 2.0 * m_kernelRadius);
    m_pressureNdensityComputeShader->setUint("DIMENSION", m_dimension);

    m_accelerationComputeShader->useProgram();
    m_accelerationComputeShader->setUint("numOfParticles", m_numOfParticles);
    m_accelerationComputeShader->setFloat("sphKernelRadius", m_kernelRadius);
    m_accelerationComputeShader->setUint("DIMENSION", m_dimension);
    m_accelerationComputeShader->setVec3("externalAccelerations", { 0.0f,-9.8f,0.0f });
    m_accelerationComputeShader->setVec3("domainRefPos", m_simulationDomain.posittion);
    m_accelerationComputeShader->setVec3("domainDimennsions", m_simulationDomain.size);
    m_accelerationComputeShader->setFloat("boundaryMaxDist", m_boundCof* m_kernelRadius);
    m_accelerationComputeShader->setUint("tableSize", m_numOfParticles);
    m_accelerationComputeShader->setFloat("cellSize", 2.0 * m_kernelRadius);
    m_accelerationComputeShader->setUint("toonerP", 7);
    m_accelerationComputeShader->setFloat("stiffnessK", 35000);
    m_accelerationComputeShader->setFloat("epsilon", 0.05f);


    m_movementComputeShader->useProgram();
    m_movementComputeShader->setFloat("timeStep", m_timeStep);
    m_movementComputeShader->setUint("numOfParticles", m_numOfParticles);
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

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_hashBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_nextNodeBuf);
    m_resetHashTableComputeShader->useProgram();
    glDispatchCompute((m_numOfParticles + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_partBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_hashBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_nextNodeBuf);
    m_initHashTableComputeShader->useProgram();
    
    glDispatchCompute((m_numOfParticles + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_partBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_fluidBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_hashBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_nextNodeBuf);
    m_pressureNdensityComputeShader->useProgram();
    glDispatchCompute((m_numOfParticles + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    m_accelerationComputeShader->useProgram();
    glDispatchCompute((m_numOfParticles + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    m_movementComputeShader->useProgram();
    glDispatchCompute((m_numOfParticles + 255) / 256, 1, 1);

    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
    glEndQuery(GL_TIME_ELAPSED);

    GLuint64 elapsedTime = 0;
    glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsedTime);
    std::cout << "Compute time (ns): " << elapsedTime << std::endl;
}