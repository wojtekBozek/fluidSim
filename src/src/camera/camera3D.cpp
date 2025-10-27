#include "camera3D.hpp"

using namespace rendering;

glm::mat4 rendering::Camera::calculateMVP(glm::mat4 model)
{
	return projection*view*model;
}

void rendering::Camera::makeViewMatrix()
{
	view = glm::lookAt(position, position + front, camera_up);
}

void rendering::Camera::updateCameraVectors()
{
	glm::vec3 front_;
	front_.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front_.y = sin(glm::radians(pitch));
	front_.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front_);
	right_dir = glm::normalize(glm::cross(front, world_up));
	camera_up = glm::normalize(glm::cross(right_dir, front));
	makeViewMatrix();
}

void rendering::Camera::returnToWorldView() {
	position = original_position;
	yaw = dCV::YAW;
	pitch = dCV::PITCH;
}

void rendering::Camera::processRotation(float xoffset, float yoffset, bool constrainPitch)
{
	yaw += xoffset;
	pitch += yoffset;

	if (constrainPitch)
	{
		if (pitch > 89.0f)	pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;
	}
	updateCameraVectors();
}

void rendering::Camera::processMovement(Camera_Movement direction, float DeltaTime)
{
	float velocity = movement_speed * DeltaTime;
	if (direction == FORWARD)
	{
		position += front * velocity;
	}
	if (direction == BACKWARD)
	{
		position -= front * velocity;
	}
	if (direction == LEFT)
	{
		position -= right_dir * velocity;
	}
	if (direction == RIGHT)
	{
		position += right_dir * velocity;
	}
	if (direction == ORIGINAL)
	{
		returnToWorldView();
	}
	updateCameraVectors();
}
