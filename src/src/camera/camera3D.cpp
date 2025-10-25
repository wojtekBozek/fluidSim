#include "camera3D.hpp"

using namespace rendering;

glm::mat4 rendering::Camera::calculateMVP(glm::mat4 model)
{
	return projection*view*model;
}
