#include "FluidSPHSimulation.hpp"
#include <iostream>
FluidSPHSimulation::FluidSPHSimulation(std::unique_ptr<ShaderProgram> computeShader, Domain simulationDomain, Fluid fluid, uint32_t numOfParticles)
: m_computeShader(std::move(computeShader))
{
    m_simulationDomain = simulationDomain;
    m_fluid = fluid;
    m_numOfParticles = numOfParticles;
    m_particles.reserve(numOfParticles);
}

void FluidSPHSimulation::setFluidAndParticles()
{
    FluidParticle initialParticle;
    initialParticle.mass = m_fluid.fluidDensity * m_fluid.volume /static_cast<float>(m_numOfParticles); // masa cząstki
    float domainVolume = m_initialDomain.size.x*m_initialDomain.size.y*m_initialDomain.size.z;
    float particleDiameter = std::cbrtf(domainVolume/static_cast<float>(m_numOfParticles));
    particleRadius = particleDiameter/2.0f;
    initialParticle.position = glm::vec4(m_initialDomain.posittion + glm::vec3(particleRadius), 1.0f);
    uint32_t xMax = std::ceil(m_initialDomain.size.x/particleDiameter)-1;
    uint32_t yMax = std::ceil(m_initialDomain.size.y/particleDiameter)-1;
    uint32_t zMax = std::ceil(m_initialDomain.size.z/particleDiameter)-1;
    
    uint32_t a=0;
    uint32_t b=0;
    uint32_t c=0;
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
            if(b < yMax)
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
    // gęstość cząstki i ciśnienie TODO: przenieść do initial shadera
    //for (auto& particle : m_particles)
    //{
    //    for(auto& otherParticle : m_particles)
    //    {
    //        //if(&particle != &otherParticle) massa cząstki lokalnej wpływa na jej lokalną gęstość (ale nie siły)
    //        particle.density += otherParticle.mass * CubicSplineKernel(DIMENSION_3, m_kernelRadius, glm::distance(particle.position, otherParticle.position));
    //    }
    //    // ciśnienie cząstki
    //    particle.pressure = m_fluid.soundSpeed*m_fluid.soundSpeed*(particle.density-m_fluid.fluidDensity); // uproszczone równanie z założeniami
    //}
}

const Fluid &FluidSPHSimulation::getFluid() const
{
    return m_fluid;
}

const uint32_t &FluidSPHSimulation::getNumOfParticles() const
{
    return m_numOfParticles;
}

const std::vector<FluidParticle> &FluidSPHSimulation::getParticles() const
{
    return m_particles;
}
