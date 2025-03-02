#pragma once
#include "headers/camera/camera3D.hpp"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <memory>
#include <list>
#include <mutex>
#include "keyboard.hpp"
namespace rendering {	
	class CameraHandler {
	public:
		CameraHandler(CameraHandler& other) = delete;
		void operator=(CameraHandler& other) = delete;

		static CameraHandler* getInstance() {
			std::lock_guard<std::mutex> lock(mtx_ASC);
			if (ASC_ptr == nullptr) {
				ASC_ptr = new CameraHandler();
			}
			return ASC_ptr;
		}

		static void setActiveCamera(std::unique_ptr<PerspectiveCamera> camera) 
		{
			CameraHandler* handler = CameraHandler::getInstance();
			handler->setCamera(std::move(camera));
		}

		
		static void processMovement(GLFWwindow* window) 
		{
			CameraHandler* handler = CameraHandler::getInstance();
			ProcessInput(window, handler->key_up, GLFW_PRESS, CameraHandler::moveUp);
			ProcessInput(window, handler->key_down, GLFW_PRESS, CameraHandler::moveDown);
			ProcessInput(window, handler->key_left, GLFW_PRESS, CameraHandler::moveLeft);
			ProcessInput(window, handler->key_right, GLFW_PRESS, CameraHandler::moveRight);
		}
		static void setCurrentSpeed(float value) 
		{
			CameraHandler* handler = CameraHandler::getInstance();
			handler->setSpeed(value);
		}
		static glm::mat4 calculateMVP(glm::mat4 mat) 
		{
			CameraHandler* handler = CameraHandler::getInstance();
			return handler->returnActiveMVP(mat);
		}

		static void connectCallbacs(GLFWwindow* window)
		{
			CameraHandler* handler = CameraHandler::getInstance();
			glfwSetWindowUserPointer(window, handler);
			glfwSetCursorPosCallback(window, CameraHandler::mouseCallback);
			glfwSetKeyCallback(window, CameraHandler::KeyboardCallback);
		}
		static void SetMouseCallbacks(GLFWwindow* window)
		{
			glfwSetCursorPosCallback(window, CameraHandler::mouseCallback);
			//glfwSetScrollCallback(window, scroll_callback);
		}

		static void setKeyboardCallback(GLFWwindow* window)
		{
			glfwSetKeyCallback(window, CameraHandler::KeyboardCallback);
		}
		static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
		{
			
			CameraHandler* handler = CameraHandler::getInstance();
			
			float xpos = static_cast<float>(xposIn);
			float ypos = static_cast<float>(yposIn);

			if (handler->first_mouse)
			{
				handler->last_x = xpos;
				handler->last_y = ypos;
				handler->first_mouse = false;
			}
			float xoffset = xpos - handler->last_x;
			float yoffset = ypos - handler->last_y;
			handler->last_x = xpos;
			handler->last_y = ypos;
			if (handler->rotation_on) {
				handler->camera_ptr->processRotation(xoffset * handler->mouse_sensitivity, -yoffset * handler->mouse_sensitivity);
			}
		}
		static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
		{
			CameraHandler* handler = CameraHandler::getInstance();
			//handler->camera_ptr->processRotation(static_cast<float>(yoffset));
		}
		static void CallbackKEY(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			
		}

		~CameraHandler() {}
	private:
		static void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
		{
			CameraHandler* handler = CameraHandler::getInstance();
			if (key == handler->key_rotation_on && action == handler->activation_type)
			{
				handler->rotation_on = !handler->rotation_on;
				if(handler->rotation_on)
				{
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}
				else
				{
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
			}
		}

		void setCamera(std::unique_ptr<PerspectiveCamera> camera)
		{
			camera_ptr = std::move(camera);
		}
		void setSpeed(float delta_time_) { delta_time = delta_time_; }
		glm::mat4 returnActiveMVP(glm::mat4 model) 
		{ 
			if (camera_ptr.get() != nullptr)
				return camera_ptr->calculateMVP(model);
			else 
				return glm::mat4(1.0f); 
		}
		CameraHandler() {
			camera_ptr = nullptr;
			key_down = GLFW_KEY_S;
			key_up = GLFW_KEY_W;
			key_left = GLFW_KEY_A;
			key_right = GLFW_KEY_D;
			key_rotation_on = GLFW_KEY_TAB;
			delta_time = 0.0f;
			mouse_sensitivity = 0.1f;
			first_mouse = true;
			rotation_on = true;
		}
		static void moveUp() { CameraHandler* handler = CameraHandler::getInstance(); if (handler->camera_ptr.get() != nullptr)handler->camera_ptr->processMovement(FORWARD, handler->delta_time); }
		static void moveDown() { CameraHandler* handler = CameraHandler::getInstance(); if (handler->camera_ptr.get() != nullptr)handler->camera_ptr->processMovement(BACKWARD, handler->delta_time); }
		static void moveRight() { CameraHandler* handler = CameraHandler::getInstance(); if (handler->camera_ptr.get() != nullptr)handler->camera_ptr->processMovement(RIGHT, handler->delta_time); }
		static void moveLeft() { CameraHandler* handler = CameraHandler::getInstance(); if (handler->camera_ptr.get() != nullptr)handler->camera_ptr->processMovement(LEFT, handler->delta_time); }
		static void origin() { CameraHandler* handler = CameraHandler::getInstance(); if (handler->camera_ptr.get() != nullptr)handler->camera_ptr->processMovement(ORIGINAL, handler->delta_time); }
		std::unique_ptr<PerspectiveCamera> camera_ptr;
		float delta_time;
		int key_up, key_down, key_left, key_right, key_origin, key_rotation_on;
		int activation_type = GLFW_PRESS;;
		float mouse_sensitivity;
		float last_x = 0.0f, last_y = 0.0f;
		bool first_mouse, rotation_on;
		static CameraHandler* ASC_ptr;
		static std::mutex mtx_ASC;
	};	
	std::mutex CameraHandler::mtx_ASC;
	CameraHandler* CameraHandler::ASC_ptr{ nullptr };
}