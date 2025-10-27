#pragma once
#include "camera3D.hpp"

namespace rendering
{
	class PerspectiveCamera : public Camera {
	public:
		PerspectiveCamera(glm::vec3 position_, glm::vec3 up_, float aspect_ratio_ = 3.0f / 4.0f,
			float near_plane_ = 0.1f, float far_plane_ = 100.0f, GLFWwindow* window_ = nullptr, float fov_ = dCV::ZOOM, float yaw_ = dCV::YAW, float pitch_ = dCV::PITCH);
		PerspectiveCamera(const PerspectiveCamera& camera);
		~PerspectiveCamera() {}
		CameraType getCameraType() const override { return CameraType::perspective; }
		float getFOV() const { return fov; }

		void processScrollback(float yOffset) override
		{
			fov -= yOffset;                  // scroll up = zoom in
			fov = glm::clamp(fov, 1.0f, 90.0f);
			makeProjectionMatrix();
		}

		void updateWindowProperties() override
		{

			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			if (windowWidth != width || height != windowHeight)
			{
				windowWidth = width;
				windowHeight = height;
				aspect_ratio = float(width) / float(height);
				makeProjectionMatrix();
			}
		}
	private:
		float fov, aspect_ratio, near_plane, far_plane;

		void makeProjectionMatrix() override
		{
			projection = glm::perspective(fov * 3.1415f / 180.0f, aspect_ratio, near_plane, far_plane);
		}
	};
}
