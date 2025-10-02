#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
namespace rendering{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		ORIGINAL
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

	class camera {
	public:
		virtual glm::mat4 calculateMVP(glm::mat4 model) = 0;
		virtual void setView(glm::vec3 view) = 0;
		virtual glm::vec3 getViewPosition() const = 0;
		virtual glm::mat4 getProjection() const = 0;
		virtual glm::mat4 getView() const = 0;
	};
	
	class PerspectiveCamera : camera {
	public:

		glm::vec3 getRightVector() const { return right_dir; }
		glm::vec3 getUpVector() const { return camera_up; }
		glm::mat4 calculateMVP(glm::mat4 model) override;
		virtual void setView(glm::vec3 view) override {};
		virtual glm::vec3 getViewPosition() const override { return position; };
		virtual glm::mat4 getProjection() const override { return projection; };
		virtual glm::mat4 getView() const override { return view; };
		void setLocation(glm::vec3 location) {};
		void setUpVector(glm::vec3 up) {};
		void setRightVector(glm::vec3 right) {};
		void setMovementSpeed(float value);
		void returnToWorldView() {
			position = original_position;
			yaw = dCV::YAW;
			pitch = dCV::PITCH;
		}

		void processRotation(float xoffset, float yoffset, bool constrainPitch = true)
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

		void processMovement(Camera_Movement direction, float DeltaTime)
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
		/*
		PerspectiveCamera(glm::vec3 position_, glm::vec3 up_, float aspect_ratio_, float near_plane_, float far_plane_, float fov_) 
		{
			original_position = position_;
			position = position_;
			world_up = up_;
			yaw = dCV::YAW;
			pitch = dCV::PITCH;
			aspect_ratio = aspect_ratio_;
			near_plane = near_plane_;
			far_plane = far_plane_;
			fov = fov_;
			makeProjectionMatrix();
			updateCameraVectors();
		}*/
		PerspectiveCamera(glm::vec3 position_, glm::vec3 up_,float aspect_ratio_ = 3.0f / 4.0f,
			float near_plane_ = 0.1f, float far_plane_ = 100.0f, float fov_ = dCV::ZOOM, float yaw_ = dCV::YAW, float pitch_ = dCV::PITCH)
			:front(glm::vec3(0.0, 0.0f, -1.0f))
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
			front_.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			front_.y = sin(glm::radians(pitch));
			front_.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			front = glm::normalize(front_);
			right_dir = glm::normalize(glm::cross(front, world_up));
			makeProjectionMatrix();
			updateCameraVectors();
		}
		PerspectiveCamera(const PerspectiveCamera& camera) :
			original_position (camera.position),
			position (camera.position),
			world_up (camera.world_up),
			yaw(camera.yaw),
			pitch( camera.pitch),
			aspect_ratio(camera.aspect_ratio),
			near_plane( camera.near_plane),
			far_plane (camera.far_plane),
			fov(camera.fov),
			front(camera.front),
			right_dir(camera.right_dir)
		{
			makeProjectionMatrix();
			updateCameraVectors();
		}
		~PerspectiveCamera() {}
	private:
		glm::vec3 position, front, camera_up, right_dir;
		glm::vec3 original_position, world_up;
		glm::mat4 view, projection;
		float fov, aspect_ratio, near_plane, far_plane;
		float yaw, pitch;
		float movement_speed = 2.5f;

		void makeViewMatrix()
		{
			view = glm::lookAt(position, position + front, camera_up);
		}
		void makeProjectionMatrix() 
		{
			projection = glm::perspective(fov, aspect_ratio, near_plane, far_plane);
		}

		void updateCameraVectors()
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
	};
}