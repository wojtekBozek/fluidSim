#pragma once
#include <glm/glm.hpp>

struct PositionedLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};