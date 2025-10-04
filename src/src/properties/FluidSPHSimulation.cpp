#include "FluidSPHSimulation.hpp"

FluidSPHSimulation::FluidSPHSimulation(std::unique_ptr<ShaderProgram> computeShader, Domain simulationDomain, Fluid fluid, uint32_t numOfParticles)
{
    m_computeShader = std::make_unique<ShaderProgram>(std::move(computeShader));
    m_simulationDomain = simulationDomain;
    m_fluid = fluid;
    m_numOfParticles = numOfParticles;
    m_particles.reserve(numOfParticles);
}

void FluidSPHSimulation::setFluidAndParticles()
{
    FluidParticle particle;
    particle.mass = m_fluid.fluidDensity * m_fluid.volume /static_cast<float>(m_numOfParticles);
    float domainVolume = m_initialDomain.size.x*m_initialDomain.size.y*m_initialDomain.size.z;
    float particleDiameter = std::cbrt(domainVolume/m_numOfParticles);
    float particleRadius = particleDiameter/2;
    particle.position = m_initialDomain.posittion + glm::vec3(particleDiameter/2.0f);
    uint32_t xMax = std::floor(m_initialDomain.size.x/particleDiameter);
    uint32_t yMax = std::floor(m_initialDomain.size.y/particleDiameter);
    uint32_t zMax = std::floor(m_initialDomain.size.z/particleDiameter);
    while(yMax*xMax*zMax < m_numOfParticles)
    {
        if(xMax <= yMax && xMax <= zMax)
        {
            xMax++;
        }
        else if(yMax <= xMax && yMax <= zMax)
        {
            yMax++;
        }
        else if(zMax <= xMax && zMax <= yMax)
        {
            zMax++;
        }
    }
    
    uint32_t a =0, b=0, c=0;
    //TODO dodać a,b,c i pierwszą logikę populowania domeny płynu
    for(int i=0; i<m_numOfParticles; ++i)
    {
        particle.position += glm::vec3(a*particleDiameter, b*particleDiameter, c*particleDiameter);
        m_particles.push_back(particle);
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
        
}
