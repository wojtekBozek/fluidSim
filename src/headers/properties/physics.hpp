#pragma once
#include "properties.hpp"
#include <glm/glm.hpp>

/**
 * @brief 
 * 
 */
class PhysicalProperties : public Property
{
    public:
    PhysicalProperties(float initMass, glm::vec3 position, glm::vec3 speed, glm::vec3 initForce);
    void applyForces(const glm::vec3& force);
    void setForce(const glm::vec3& force);
    void updatePhysics(float deltaTime);
    void forcePosition(glm::vec3 position);
    void setMass(float newMass);
    glm::vec3 getPosition() const;
    ~PhysicalProperties(){}
    private:
    glm::vec3 m_position;
    glm::vec3 m_speed;
    glm::vec3 m_forces;
    float m_mass;
};