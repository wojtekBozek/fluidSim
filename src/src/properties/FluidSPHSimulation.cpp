#include "FluidSPHSimulation.hpp"
#include <iostream>
#include <debug_data.hpp>
FluidSPHSimulation::FluidSPHSimulation(std::unique_ptr<ShaderProgram> computePressureShader, std::unique_ptr<ShaderProgram> computeMovementShader, Domain simulationDomain, Fluid fluid, uint32_t numOfParticles)
: m_pressureNdensityComputeShader(std::move(computePressureShader)), m_movementComputeShader(std::move(computeMovementShader))
{
    m_simulationDomain = simulationDomain;
    m_fluid = fluid;
    refFluid = fluid;
    setInitialState();
}

void FluidSPHSimulation::setInitialState()
{
    m_fluid = refFluid;
    if (m_dimension == SimDim::DIMENSION_3)
    {
        m_initialDomain.position = glm::vec3(-2.5 + 4 * m_particleRadius, 0.0 + 4 * m_particleRadius, -1.0 + 0.4 * m_particleRadius);

        m_initialDomain.size = glm::vec3(5.0, 5.0, 2.5);
        m_simulationDomain.position = glm::vec3(-5.0, -2.5, -5.0);

        m_simulationDomain.size = glm::vec3(10.0, 10.0, 10.0);
        m_fluid.volume = m_initialDomain.size.x * m_initialDomain.size.y * m_initialDomain.size.z;
        m_numOfParticles = m_fluid.volume / (1.33 * std::pow(m_particleRadius, 3) * M_PI);
    }
    else if (m_dimension == SimDim::DIMENSION_2)
    {
        m_initialDomain.position = glm::vec3(-10.0, 0.0, 0.0);

        m_initialDomain.size = glm::vec3(20.0, 10.0, 0.0);

        m_simulationDomain.position = glm::vec3(-15.0, -2.5, 0.0);
        m_simulationDomain.size = glm::vec3(30.0, 20.0, 0.0);
        //m_fluid.fluidDensity *= m_particleRadius;
        m_fluid.volume = m_initialDomain.size.x * m_initialDomain.size.y * m_particleRadius*2;// *m_initialDomain.size.z;
        m_numOfParticles = m_fluid.volume / (std::pow(m_particleRadius, 2) * M_PI);
    }
    m_kernelRadius = m_kernelCof * m_particleRadius;
}

void FluidSPHSimulation::setInitialSimulationDomain()
{
    if (m_dimension == SimDim::DIMENSION_3)
    {
        m_initialDomain.position = glm::vec3(-2.5 + 4 * m_particleRadius, 0.0 + 4 * m_particleRadius, -1.0 + 0.4 * m_particleRadius);

        m_initialDomain.size = glm::vec3(5.0, 5.0, 2.5);
        m_simulationDomain.position = glm::vec3(-5.0, -2.5, -5.0);

        m_simulationDomain.size = glm::vec3(10.0, 10.0, 10.0);
    }
    else if (m_dimension == SimDim::DIMENSION_2)
    {
        m_initialDomain.position = glm::vec3(-10.0, 0.0, 0.0);

        m_initialDomain.size = glm::vec3(20.0, 10.0, 0.0);
        m_simulationDomain.position = glm::vec3(-15.0, -2.5, 0.0);
        m_simulationDomain.size = glm::vec3(30.0, 20.0, 0.0);
    }
}

void FluidSPHSimulation::setSimulationState()
{
    if (m_dimension == SimDim::DIMENSION_3)
    {
        m_fluid.volume = m_initialDomain.size.x * m_initialDomain.size.y * m_initialDomain.size.z;
        m_numOfParticles = m_fluid.volume / (1.33 * std::pow(m_particleRadius, 3) * M_PI);
    }
    else if (m_dimension == SimDim::DIMENSION_2)
    {
        //m_fluid.fluidDensity *= m_particleRadius;
        m_fluid.volume = m_initialDomain.size.x * m_initialDomain.size.y;// *m_initialDomain.size.z;
        m_numOfParticles = m_fluid.volume / (std::pow(m_particleRadius, 2) * M_PI);
    }
    m_kernelRadius = m_kernelCof * m_particleRadius;
}

void FluidSPHSimulation::setFluidAndParticles()
{    
    m_particles.clear();
    FluidParticle initialParticle;
    float particleDiameter = m_particleRadius *1.95f;
    initialParticle.mass = (m_fluid.fluidDensity * m_fluid.volume) /static_cast<float>(m_numOfParticles); // masa cząstki
    std::cout << "Num of particles: " << m_numOfParticles << "\n";
    std::cout << m_particleRadius << ", " << initialParticle.mass << " - particle radius, mass\n";
    initialParticle.position = glm::vec4(m_initialDomain.position + glm::vec3(particleDiameter), 1.0f);
    
    if (SimDim::DIMENSION_2 == m_dimension)
    {
        initialParticle.position.z = 0.0;
    }
    initialParticle.velocity = glm::vec4(glm::vec3(0.0f), 1.0f);
    initialParticle.acceleration = glm::vec4(glm::vec3(0.0f), 1.0f);
    initialParticle.type = particleType::FLUID;
    uint32_t xMax = std::ceil(m_initialDomain.size.x/(particleDiameter));
    uint32_t yMax = std::ceil(m_initialDomain.size.y/(particleDiameter))-1;
    uint32_t zMax = 0;

    uint32_t boundaryXmax = m_simulationDomain.size.x/(particleDiameter);
    uint32_t boundaryYmax = m_simulationDomain.size.y/(particleDiameter);
    uint32_t boundaryZmax = m_simulationDomain.size.z/(particleDiameter);
    numOfBoundaryParticles = 2*boundaryXmax + 2*boundaryYmax;
    
    m_numOfParticles = xMax*yMax;
    if (m_dimension == SimDim::DIMENSION_3)
    {
        zMax = std::ceil(m_initialDomain.size.z / (particleDiameter)) - 1;
        numOfBoundaryParticles += 2 * boundaryZmax;
        
        m_numOfParticles = xMax*yMax*zMax;
    }
    uint32_t a=0;
    uint32_t b=0;
    uint32_t c=0;
    m_particles.reserve(m_numOfParticles + numOfBoundaryParticles);
    for(int i=0; i<m_numOfParticles; ++i)
    {
        FluidParticle part = initialParticle;
        part.position += glm::vec4(glm::vec3(a*(particleDiameter), b*(particleDiameter), c*(particleDiameter)),0.0f);
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
    std::vector<FluidParticle> boundaryParticles;


    boundaryParticles.reserve(numOfBoundaryParticles);
    initialParticle.position = glm::vec4(m_simulationDomain.position, 1.0f);
    //initialParticle.density = 1000.0/static_cast<float>(numOfBoundaryParticles);
    initialParticle.type = particleType::BOUNDARY;

    int depth = m_boundaryDepth;
    if (SimDim::DIMENSION_2 == m_dimension)
    {
        initialParticle.position.z = m_initialDomain.position.z;
        initialParticle.mass = m_fluid.fluidDensity*m_kernelRadius*m_kernelRadius;
        for (int i = -depth; i <= int(boundaryXmax)+depth; i++)
        {
            for (int j = -depth; j <= int(boundaryYmax)+depth; j++)
            {
                if (i <= 0 || i >= boundaryXmax || j <= 0 || j >= boundaryYmax)
                {
                    initialParticle.position = glm::vec4(m_simulationDomain.position + glm::vec3(i * (particleDiameter), j * (particleDiameter), 0.0), 1.0);
                    boundaryParticles.push_back(initialParticle);
                }
            }
        }
    } 
    else if (SimDim::DIMENSION_3 == m_dimension)
    {
        
        initialParticle.mass = m_fluid.fluidDensity*m_kernelRadius*m_kernelRadius*m_kernelRadius;
        for (int i = -depth; i <= int(boundaryXmax)+depth; i++)
        {
            for (int j = -depth; j <= int(boundaryYmax)+depth; j++)
            {
                for (int z = -depth; z <= int(boundaryZmax)+depth; z++)
                    if (i <= 0 || i >= boundaryXmax  || j <= 0 || j >= boundaryYmax  || z <= 0 || z >= boundaryZmax)
                    {
                        initialParticle.position =  glm::vec4(m_simulationDomain.position + glm::vec3(i * (particleDiameter), j * (particleDiameter), z * (particleDiameter)), 1.0);
                        boundaryParticles.push_back(initialParticle);
                    }
            }
        }
    }
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    GLuint bufsize = boundaryParticles.size() * sizeof(FluidParticle);
    numOfBoundaryParticles = boundaryParticles.size();

    m_particles.insert(m_particles.end(), std::make_move_iterator(boundaryParticles.begin()), std::make_move_iterator(boundaryParticles.end()));
    m_numOfParticles = m_particles.size();
    std::cout << m_numOfParticles << "\n";
}

void FluidSPHSimulation::setMemoryLayout()
{
    setParticleBufferData();
    GLuint bufsize = sizeof(Fluid);
    if (glIsBuffer(m_fluidBuf) == GL_FALSE)
    {
        glGenBuffers(1, &m_fluidBuf);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_fluidBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, &m_fluid, GL_DYNAMIC_DRAW);

    bufsize = m_numOfParticles * sizeof(glm::vec4);
    if (glIsBuffer(m_velocitiesBuf) == GL_FALSE)
    {
        glGenBuffers(1, &m_velocitiesBuf);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_velocitiesBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, hashCoef * bufsize, nullptr, GL_DYNAMIC_DRAW);
    m_hashValues = std::vector<GLint>(hashCoef * m_numOfParticles, -1);
    m_nextNodes = std::vector<GLint>(m_numOfParticles, -1);

    bufsize = m_numOfParticles * sizeof(GLint);
    if (glIsBuffer(m_hashBuf) == GL_FALSE)
    {
        glGenBuffers(1, &m_hashBuf);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_hashBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, hashCoef * bufsize, m_hashValues.data(), GL_DYNAMIC_DRAW);
    if (glIsBuffer(m_nextNodeBuf) == GL_FALSE)
    {
        glGenBuffers(1, &m_nextNodeBuf);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nextNodeBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, m_nextNodes.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    


    m_resetHashTableComputeShader->useProgram();
    m_resetHashTableComputeShader->setUint("numOfParticles", m_numOfParticles);
    m_resetHashTableComputeShader->setUint("tableSize", hashCoef * m_numOfParticles);

    m_initHashTableComputeShader->useProgram();
    m_initHashTableComputeShader->setUint("numOfParticles", m_numOfParticles);
    m_initHashTableComputeShader->setUint("tableSize", hashCoef * m_numOfParticles);
    m_initHashTableComputeShader->setFloat("cellSize", 2.0 * m_kernelRadius);

    m_pressureNdensityComputeShader->useProgram();
    m_pressureNdensityComputeShader->setUint("numOfParticles", m_numOfParticles);
    m_pressureNdensityComputeShader->setFloat("sphKernelRadius", m_kernelRadius);
    m_pressureNdensityComputeShader->setUint("tableSize", hashCoef * m_numOfParticles);
    m_pressureNdensityComputeShader->setFloat("cellSize", 2.0 * m_kernelRadius);
    m_pressureNdensityComputeShader->setUint("DIMENSION", m_dimension);

    m_accelerationComputeShader->useProgram();
    m_accelerationComputeShader->setUint("numOfParticles", m_numOfParticles);
    m_accelerationComputeShader->setFloat("sphKernelRadius", m_kernelRadius);
    m_accelerationComputeShader->setFloat("particleRadius", m_particleRadius);
    m_accelerationComputeShader->setUint("DIMENSION", m_dimension);
    m_accelerationComputeShader->setVec3("externalAccelerations", { 0.0f,-9.8f,0.0f });
    m_accelerationComputeShader->setVec3("domainRefPos", m_simulationDomain.position);
    m_accelerationComputeShader->setVec3("domainDimennsions", m_simulationDomain.size);
    m_accelerationComputeShader->setFloat("boundaryMaxDist", m_boundCof * m_kernelRadius);
    m_accelerationComputeShader->setUint("tableSize", hashCoef * m_numOfParticles);
    m_accelerationComputeShader->setFloat("cellSize", 2.0 * m_kernelRadius);
    m_accelerationComputeShader->setFloat("wallStiffness", 10000.0);
    m_accelerationComputeShader->setFloat("epsilon", m_smooothingEpsilon);
    m_accelerationComputeShader->setFloat("epsilonBoundary", m_smooothingEpsilon);
    m_accelerationComputeShader->setUint("borderPolicy", m_borderPolicy);

    //m_smoothingComputeShader->useProgram();
    //m_smoothingComputeShader->setUint("numOfParticles", m_numOfParticles);
    //m_smoothingComputeShader->setFloat("sphKernelRadius", m_kernelRadius);
    //m_smoothingComputeShader->setUint("DIMENSION", m_dimension);
    //m_smoothingComputeShader->setVec3("externalAccelerations", { 0.0f,-9.8f,0.0f });
    //m_smoothingComputeShader->setVec3("domainRefPos", m_simulationDomain.position);
    //m_smoothingComputeShader->setVec3("domainDimennsions", m_simulationDomain.size);
    //m_smoothingComputeShader->setFloat("boundaryMaxDist", m_boundCof * m_kernelRadius);
    //m_smoothingComputeShader->setUint("tableSize", hashCoef * m_numOfParticles);
    //m_smoothingComputeShader->setFloat("cellSize", 2.0 * m_kernelRadius);
    //m_smoothingComputeShader->setUint("toonerP", 7);
    //m_smoothingComputeShader->setFloat("stiffnessK", 35000);
    //m_smoothingComputeShader->setFloat("epsilon", 0.1f);
    //m_smoothingComputeShader->setFloat("epsilonBoundary", 0.15f);
    //m_smoothingComputeShader->setUint("borderPolicy", m_borderPolicy);

    m_movementComputeShader->useProgram();
    m_movementComputeShader->setFloat("timeStep", m_timeStep);
    m_movementComputeShader->setUint("numOfParticles", m_numOfParticles);
}

void FluidSPHSimulation::setParticleBufferData()
{
    if (!m_partBufferCreated)
    {
        if (glIsBuffer(m_partBuf) == GL_FALSE)
        {
            glGenBuffers(1, &m_partBuf);
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_partBuf);
        m_partBufferCreated = true;
    }
    GLuint bufsize = m_numOfParticles * sizeof(FluidParticle);
    if (bufsize != m_particleBufferSize)
    {
        Debug("Creating particle Buffer");
        m_particleBufferSize = bufsize;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_partBuf);
        glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, m_particles.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_partBuf);
        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
    }
}

void FluidSPHSimulation::simulationStep(float timeStep)
{
    GLuint query;
    glGenQueries(1, &query);

    
    GLuint64 temp = 0;
    uint32_t steps = 0;
    while(temp < 16000000 && steps*m_timeStep < 0.0166){
        glBeginQuery(GL_TIME_ELAPSED, query);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_hashBuf);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_nextNodeBuf);
        m_resetHashTableComputeShader->useProgram();
        glDispatchCompute((m_numOfParticles + MinInvocations-1) / MinInvocations, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_partBuf);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_hashBuf);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_nextNodeBuf);
        m_initHashTableComputeShader->useProgram();
        
        glDispatchCompute((m_numOfParticles + MinInvocations - 1) / MinInvocations, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_partBuf);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_fluidBuf);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_hashBuf);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_nextNodeBuf);
        m_pressureNdensityComputeShader->useProgram();
        glDispatchCompute((m_numOfParticles + MinInvocations - 1) / MinInvocations, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        m_accelerationComputeShader->useProgram();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_velocitiesBuf);
        glDispatchCompute((m_numOfParticles + MinInvocations - 1) / MinInvocations, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        m_movementComputeShader->useProgram();
        glDispatchCompute((m_numOfParticles + MinInvocations - 1) / MinInvocations, 1, 1);

        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        glEndQuery(GL_TIME_ELAPSED);

        glGetQueryObjectui64v(query, GL_QUERY_RESULT, &m_computeTime);
        temp += m_computeTime;
        steps++;
        m_computationStep++;
    }
}

void FluidSPHSimulation::clearSimulation()
{
    m_computationStep = 0;
    m_particles.clear();
    m_partBufferCreated = false;
    m_particleBufferSize = 0;
}
