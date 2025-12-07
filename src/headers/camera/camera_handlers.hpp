#pragma once
#include "camera3D.hpp"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <memory>
#include <list>
#include <mutex>
#include "keyboard.hpp"
#include <imgui.h>

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

		std::shared_ptr<Camera> getCamera() const 
		{
			return camera_ptr;
		}

		static void addCamera(std::shared_ptr<Camera> camera)
		{
			CameraHandler* handler = CameraHandler::getInstance();
			if(camera)
				handler->cameras.push_back(camera);
		}
		static void addActiveCamera(std::shared_ptr<Camera> camera) 
		{
			CameraHandler* handler = CameraHandler::getInstance();
			if (camera)
			{
				handler->setCamera(camera);
				handler->addCamera(camera);
			}
		}

		static void setActiveCamera(int id)
		{
			CameraHandler* handler = CameraHandler::getInstance();
			if (id < handler->cameras.size())
			{
				auto camera = handler->cameras.at(id);
				handler->changeCameraInPlace(camera);
			}
		}

		static void changeCameraInPlace(std::shared_ptr<Camera> camera) 
		{
			CameraHandler* handler = CameraHandler::getInstance();
			std::shared_ptr<Camera> oldCamera = handler->getCamera();
			camera->setView(oldCamera->getView());
			camera->setPitch(oldCamera->getPitch());
			camera->setYaw(oldCamera->getYaw());
			camera->setUpVector(oldCamera->getUpVector());
			camera->setPosition(oldCamera->getPosition());
			camera->updateWindowProperties();
			camera->updateCameraVectors();
			handler->setCamera(camera);
		}

		static void resizeCallback(GLFWwindow* window, int width, int height)
		{
			CameraHandler* handler = CameraHandler::getInstance();
			glViewport(0, 0, width, height);
			handler->getCamera()->updateWindowProperties();
		}

		static void processMovement(GLFWwindow* window) 
		{
			ImGuiIO& io = ImGui::GetIO();
			if (io.WantCaptureKeyboard)	return;

			CameraHandler* handler = CameraHandler::getInstance();
			ProcessInput(window, handler->key_forward, GLFW_PRESS, CameraHandler::moveForward);
			ProcessInput(window, handler->key_backward, GLFW_PRESS, CameraHandler::moveBackward);
			ProcessInput(window, handler->key_vertical_up, GLFW_PRESS, CameraHandler::moveVerticallyUp);
			ProcessInput(window, handler->key_vertical_down, GLFW_PRESS, CameraHandler::moveVerticallyDown);
			ProcessInput(window, handler->key_left, GLFW_PRESS, CameraHandler::moveLeft);
			ProcessInput(window, handler->key_right, GLFW_PRESS, CameraHandler::moveRight);
			ProcessInput(window, handler->key_origin, GLFW_PRESS, CameraHandler::origin); 
			ProcessInput(window, handler->key_camera1, GLFW_PRESS, CameraHandler::setActiveCamera, 0);
			ProcessInput(window, handler->key_camera2, GLFW_PRESS, CameraHandler::setActiveCamera, 1);
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

		static void connectCallbacks(GLFWwindow* window)
		{
			CameraHandler* handler = CameraHandler::getInstance();
			glfwSetWindowUserPointer(window, handler);
			glfwSetCursorPosCallback(window, CameraHandler::mouseCallback);
			glfwSetScrollCallback(window, CameraHandler::scrollCallback);
			glfwSetKeyCallback(window, CameraHandler::KeyboardCallback);
			glfwSetFramebufferSizeCallback(window, CameraHandler::resizeCallback);
		}
		static void SetMouseCallbacks(GLFWwindow* window)
		{
			glfwSetCursorPosCallback(window, CameraHandler::mouseCallback);
			glfwSetScrollCallback(window, CameraHandler::scrollCallback);
		}

		static void setKeyboardCallback(GLFWwindow* window)
		{
			glfwSetKeyCallback(window, CameraHandler::KeyboardCallback);
		}
		static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
		{
			ImGuiIO& io = ImGui::GetIO();
			if (io.WantCaptureMouse) return;
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
				if(handler->camera_ptr)
					handler->camera_ptr->processRotation(xoffset * handler->mouse_sensitivity, -yoffset * handler->mouse_sensitivity);
			}
		}
		static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
		{
			ImGuiIO& io = ImGui::GetIO();
			if (io.WantCaptureMouse) return;
			CameraHandler* handler = CameraHandler::getInstance();
			if(handler->camera_ptr)
				handler->camera_ptr->processScrollback(static_cast<float>(yoffset));
		}
		static void CallbackKEY(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			
		}

		bool isRotationOn() const
		{
			return rotation_on;
		}

		~CameraHandler() {}
	private:
		static void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
		{
			ImGuiIO& io = ImGui::GetIO();
			if (io.WantCaptureKeyboard)	return;

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

		void setCamera(std::shared_ptr<Camera> camera)
		{
			camera_ptr = camera;
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
			key_backward = GLFW_KEY_S;
			key_forward = GLFW_KEY_W;
			key_left = GLFW_KEY_A;
			key_right = GLFW_KEY_D;
			key_origin = GLFW_KEY_O;
			key_vertical_down = GLFW_KEY_PAGE_DOWN;
			key_vertical_up = GLFW_KEY_PAGE_UP;
			key_rotation_on = GLFW_KEY_TAB;
			key_camera1 = GLFW_KEY_F1;
			key_camera2 = GLFW_KEY_F2;
			delta_time = 0.0f;
			mouse_sensitivity = 0.1f;
			first_mouse = true;
			rotation_on = false;
		}
		static void moveForward() { CameraHandler* handler = CameraHandler::getInstance(); if (handler->camera_ptr.get() != nullptr)handler->camera_ptr->processMovement(FORWARD, handler->delta_time); }
		static void moveBackward() { CameraHandler* handler = CameraHandler::getInstance(); if (handler->camera_ptr.get() != nullptr)handler->camera_ptr->processMovement(BACKWARD, handler->delta_time); }
		static void moveVerticallyUp() { CameraHandler* handler = CameraHandler::getInstance(); if (handler->camera_ptr.get() != nullptr)handler->camera_ptr->processMovement(VERTICAL_UP, handler->delta_time); }
		static void moveVerticallyDown() { CameraHandler* handler = CameraHandler::getInstance(); if (handler->camera_ptr.get() != nullptr)handler->camera_ptr->processMovement(VERTICAL_DOWN, handler->delta_time); }
		static void moveRight() { CameraHandler* handler = CameraHandler::getInstance(); if (handler->camera_ptr.get() != nullptr)handler->camera_ptr->processMovement(RIGHT, handler->delta_time); }
		static void moveLeft() { CameraHandler* handler = CameraHandler::getInstance(); if (handler->camera_ptr.get() != nullptr)handler->camera_ptr->processMovement(LEFT, handler->delta_time); }
		static void origin() { CameraHandler* handler = CameraHandler::getInstance(); if (handler->camera_ptr.get() != nullptr)handler->camera_ptr->processMovement(ORIGINAL, handler->delta_time); }
		std::shared_ptr<Camera> camera_ptr;
		std::vector<std::shared_ptr<Camera>> cameras;
		float delta_time;
		int key_forward, key_backward, key_left, key_right, key_origin, key_rotation_on, key_vertical_up, key_vertical_down;
		int key_camera1, key_camera2;
		int activation_type = GLFW_PRESS;
		float mouse_sensitivity;
		float last_x = 0.0f, last_y = 0.0f;
		bool first_mouse, rotation_on;
		static CameraHandler* ASC_ptr;
		static std::mutex mtx_ASC;
	};	
	std::mutex CameraHandler::mtx_ASC;
	CameraHandler* CameraHandler::ASC_ptr{ nullptr };
}