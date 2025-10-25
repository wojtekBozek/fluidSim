#pragma once
//#define _USE_MATH_DEFINES
//#include <math.h>
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

	enum CameraType
	{
		perspective,
		orthographic
	};
	class Camera {
	public:
		virtual CameraType getCameraType() const = 0;
		glm::mat4 calculateMVP(glm::mat4 model);
		void setView(glm::mat4 view_) {view=view_;}
		glm::vec3 getPosition() const { return position; }
		glm::mat4 getProjection() const { return projection; }
		glm::mat4 getView() const { return view; }

		glm::vec3 getRightVector() const { return right_dir;}
		glm::vec3 getUpVector() const { return camera_up; }

		float getYaw() const {return yaw;}
		float getPitch() const {return pitch;}

		void setYaw(float yaw_) {yaw = yaw_;}
		void setPitch(float pitch_) {pitch = pitch_;}

		void setPosition(glm::vec3 location_) {position=location_;}
		void setUpVector(glm::vec3 up_) {camera_up=up_;};
		void setRightVector(glm::vec3 right_) {right_dir=right_;}
		void setMovementSpeed(float value_){movement_speed=value_;}

		void makeViewMatrix()
		{
			view = glm::lookAt(position, position + front, camera_up);
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

		void setWindowHeight(float height){windowHeight = height;}
		float getWindowHeight() const {return windowHeight;}
	protected:
		glm::vec3 position, front, camera_up, right_dir;
		glm::vec3 original_position, world_up;
		glm::mat4 view, projection;
		float yaw, pitch;
		float movement_speed = 2.5f;
		float windowHeight;
	};
	
	class PerspectiveCamera : public Camera {
	public:
		PerspectiveCamera(glm::vec3 position_, glm::vec3 up_,float aspect_ratio_ = 3.0f / 4.0f,
			float near_plane_ = 0.1f, float far_plane_ = 100.0f, float fov_ = dCV::ZOOM, float yaw_ = dCV::YAW, float pitch_ = dCV::PITCH)
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
			PerspectiveCamera(camera.position,camera.world_up,camera.aspect_ratio,
			camera.near_plane,camera.far_plane,camera.fov,camera.yaw,camera.pitch)
		{
		}
		~PerspectiveCamera() {}
		CameraType getCameraType() const override {return CameraType::perspective;}
		float getFOV() const {return fov;}
	private:
		float fov, aspect_ratio, near_plane, far_plane;

		
		void makeProjectionMatrix() 
		{
			projection = glm::perspective(fov*3.1415f/180.0f, aspect_ratio, near_plane, far_plane);
		}

		
	};

	class OrthographicCamera : public Camera
	{
		public:
		OrthographicCamera(float l, float r, float b, float t, float n, float f)
        : left(l), right_(r), bottom(b), top(t), near_plane(n), far_plane(f) 
		{
    	}

		OrthographicCamera(glm::vec3 position_, glm::vec3 up_, float l, float r, float b, float t, float n, float f,float yaw_ = dCV::YAW, float pitch_ = dCV::PITCH)
        : OrthographicCamera(l,r,b,t,n,f) 
		{
			original_position = position_;
			position = position_;
			world_up = up_;
			yaw = yaw_;
			pitch = pitch_;
        	updateProjection();
        	makeViewMatrix();
    	}

		CameraType getCameraType() const override {return CameraType::orthographic;}
		
		float getBottom() const {return bottom;}
		float getTop() const {return top;}
		
		private:
		float left, right_, bottom, top, near_plane, far_plane;
		void updateProjection() 
		{
        	projection = glm::ortho(left, right_, bottom, top, near_plane, far_plane);
    	}
	};
}