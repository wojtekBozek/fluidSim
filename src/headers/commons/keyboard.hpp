#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"

template<typename T, typename... Args>
T ProcessInput(GLFWwindow* window, int key,int action, T (*func)( Args ...), Args ... args)
{
	if (glfwGetKey(window, key) == GLFW_PRESS)
	{
		return func(args...);
	}
}

template<typename T>
T ProcessInput(GLFWwindow* window, int key, int action, T(*func)())
{
	if (glfwGetKey(window, key) == GLFW_PRESS)
	{
		return func();
	}
}

template<typename T, typename... Args>
T ProcessInput(GLFWwindow* window, int key,int action, T (*func)( Args& ...), Args& ... args)
{
	if (glfwGetKey(window, key) == action)
	{
		return func(args...);
	}
}

template<typename T, typename... Args>
T ProcessInput(GLFWwindow* window, int key,int action, T (*func)( Args* ...), Args* ... args)
{
	if (glfwGetKey(window, key) == GLFW_PRESS)
	{
		return func(args...);
	}
}

template<typename T, typename... Args>
T ProcessInput(GLFWwindow* window, int key,int action, T (*func)( Args* ..., Args ...), Args* ... args, Args ... args2)
{
	if (glfwGetKey(window, key) == action)
	{
		return func(args..., args2...);
	}
}

template<typename T, typename... Args>
T ProcessInput(GLFWwindow* window, int key,int action, T (*func)( Args* ..., Args& ...), Args* ... args, Args& ... args2)
{
	if (glfwGetKey(window, key) == action)
	{
		return func(args..., args2...);
	}
}

template<typename T, typename... Args>
T ProcessInput(GLFWwindow* window, int key,int action, T (*func)( Args** ..., Args* ...), Args** ... args, Args* ... args2)
{
	if (glfwGetKey(window, key) == action)
	{
		return func(args..., args2...);
	}
}
