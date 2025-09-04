#include "camera3D.hpp"

using namespace rendering;

glm::mat4 rendering::PerspectiveCamera::calculateMVP(glm::mat4 model)
{
	return projection*view*model;
}
