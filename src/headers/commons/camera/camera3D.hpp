#pragma once
//#define _USE_MATH_DEFINES
//#include <math.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
namespace rendering{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		ORIGINAL,
		VERTICAL_UP,
		VERTICAL_DOWN
	};

	namespace dCV
	{
		const float YAW = -90.0f;
		const float PITCH = 0.0f;
		const float SPEED = 2.5f;
		const float MINSPEED = 0.5f;
		const float MAXSPEED = 10.0f;
		const float SENSITIVITY = 0.1f;
		const float ZOOM = 45.0f;
	}

	enum CameraType
	{
		perspective,
		orthographic
	};
	class Camera {
	public:
		virtual CameraType getCameraType() const = 0;
		virtual void updateWindowProperties() = 0;
		virtual void processScrollback(float yOffset) = 0;

		glm::mat4 calculateMVP(glm::mat4 model);
		
		void makeViewMatrix();

		void updateCameraVectors();

		void returnToWorldView();

		void processRotation(float xoffset, float yoffset, bool constrainPitch = true);

		void processMovement(Camera_Movement direction, float DeltaTime);

		void setWindowHeight(float height){windowHeight = height;}
		void setView(glm::mat4 view_) {view=view_;}
		void setYaw(float yaw_) {yaw = yaw_;}
		void setPitch(float pitch_) {pitch = pitch_;}
		void setPosition(glm::vec3 location_) {position=location_;}
		void setUpVector(glm::vec3 up_) {camera_up=up_;};
		void setRightVector(glm::vec3 right_) {right_dir=right_;}
		void setMovementSpeed(float value_){movement_speed=value_;}

		float getWindowHeight() const {return windowHeight;}
		float getYaw() const {return yaw;}
		float getPitch() const {return pitch;}

		glm::mat4 getProjection() const { return projection; }
		glm::mat4 getView() const { return view; }

		glm::vec3 getPosition() const { return position; }
		glm::vec3 getRightVector() const { return right_dir; }
		glm::vec3 getUpVector() const { return camera_up; }

		GLFWwindow* getWindow() const { return window; }
	protected:
		virtual void makeProjectionMatrix() = 0;
		glm::mat4 view, projection;
		glm::vec3 position, front, camera_up, right_dir;
		glm::vec3 original_position, world_up;
		float yaw, pitch;
		float movement_speed = 2.5f;
		int windowHeight, windowWidth;
		GLFWwindow* window;
	};	
}