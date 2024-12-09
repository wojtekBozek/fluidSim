#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/type_ptr.hpp>
constexpr float epsilon = 0.000001f;

inline glm::mat4 quaternionRotation(float angle_in_radians, glm::vec3 rotation_axis) 
{
	return glm::mat4_cast(glm::quat(cos(angle_in_radians / 2),
		glm::epsilonEqual<float>(0.0f, rotation_axis.x, epsilon) ? 0.0f : rotation_axis.x * sin(angle_in_radians / 2),
		glm::epsilonEqual<float>(0.0f, rotation_axis.y, epsilon) ? 0.0f : rotation_axis.y * sin(angle_in_radians / 2),
		glm::epsilonEqual<float>(0.0f, rotation_axis.z, epsilon) ? 0.0f : rotation_axis.z * sin(angle_in_radians / 2)));
}