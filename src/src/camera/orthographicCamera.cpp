#include "orthographicCamera.hpp"

rendering::OrthographicCamera::OrthographicCamera(float l, float r, float b, float t, float n, float f)
	: left(l), right_(r), bottom(b), top(t), near_plane(n), far_plane(f)
{
}

rendering::OrthographicCamera::OrthographicCamera(glm::vec3 position_, glm::vec3 up_, float l, float r, float b, float t, float n, float f, GLFWwindow* window_, float yaw_, float pitch_)
	: OrthographicCamera(l, r, b, t, n, f)
{
	original_position = position_;
	position = position_;
	world_up = up_;
	yaw = yaw_;
	pitch = pitch_;
	window = window_;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	makeProjectionMatrix();
	makeViewMatrix();
}
