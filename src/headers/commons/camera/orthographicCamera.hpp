#pragma once

#include "camera3D.hpp"

namespace rendering 
{
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float l, float r, float b, float t, float n, float f);

		OrthographicCamera(glm::vec3 position_, glm::vec3 up_, float l, float r, float b, float t, float n, float f, GLFWwindow* window = nullptr, float yaw_ = dCV::YAW, float pitch_ = dCV::PITCH);

		CameraType getCameraType() const override { return CameraType::orthographic; }

		float getBottom() const { return bottom; }
		float getTop() const { return top; }

		void processScrollback(float yOffset) override
		{
			float zoomFactor = 1.0f + yOffset/100.0f;
			left *= zoomFactor;
			right_ *= zoomFactor;
			bottom *= zoomFactor;
			top *= zoomFactor;
			makeProjectionMatrix();
		}
		void updateWindowProperties() override
		{
			glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
		}

	private:
		float left, right_, bottom, top, near_plane, far_plane;
		void makeProjectionMatrix() override
		{
			projection = glm::ortho(left, right_, bottom, top, near_plane, far_plane);
		}
	};
}