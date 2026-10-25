#include "perspectiveCamera.hpp"

rendering::PerspectiveCamera::PerspectiveCamera(glm::vec3 position_, glm::vec3 up_, float aspect_ratio_, float near_plane_, float far_plane_, GLFWwindow* window_, float fov_, float yaw_, float pitch_)
{
	original_position = position_;
	position = position_;
	world_up = up_;
	yaw = yaw_;
	pitch = pitch_;
	aspect_ratio = aspect_ratio_;
	near_plane = near_plane_;
	far_plane = far_plane_;
	fov = fov_;
	movement_speed = 2.5f;
	glm::vec3 front_;
	window = window_;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	front_.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front_.y = sin(glm::radians(pitch));
	front_.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front_);
	right_dir = glm::normalize(glm::cross(front, world_up));
	makeProjectionMatrix();
	updateCameraVectors();
}

rendering::PerspectiveCamera::PerspectiveCamera(const PerspectiveCamera& camera) :
	PerspectiveCamera(camera.position, camera.world_up, camera.aspect_ratio,
		camera.near_plane, camera.far_plane, camera.window, camera.fov, camera.yaw, camera.pitch)
{
}
