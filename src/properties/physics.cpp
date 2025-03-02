#include "physics.hpp"

PhysicalProperties::PhysicalProperties(float initMass, glm::vec3 position, glm::vec3 speed, glm::vec3 initForce) 
: m_mass(initMass), m_position(position), m_speed(speed), m_forces(initForce)
{
    assert(m_mass > 0.0f);
}

void PhysicalProperties::applyForces(const glm::vec3 &force)
{
    m_forces += force;
}

void PhysicalProperties::setForce(const glm::vec3 &force)
{
    m_forces = force;
}

void PhysicalProperties::updatePhysics(float deltaTime)
{
    glm::vec3 acceleration = m_forces/m_mass;
    m_speed += acceleration * deltaTime;
    m_position += m_speed * deltaTime;
    m_forces = glm::vec3(0.0f);
}

void PhysicalProperties::forcePosition(glm::vec3 position)
{
    m_position = position;
}

void PhysicalProperties::setMass(float newMass)
{
    assert(newMass > 0.0f);
    m_mass = newMass;
}

glm::vec3 PhysicalProperties::getPosition() const
{
    return m_position;
}
