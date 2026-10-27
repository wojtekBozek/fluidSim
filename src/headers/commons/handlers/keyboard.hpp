#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <functional>
#include <type_traits>
#include <utility>
#include <optional>
#include <concepts>

template<typename F, typename... Args>
	requires std::invocable<F, Args...>
decltype(auto) ProcessInput(GLFWwindow* window, int key, int action, F&& func,Args&&... args) 
{
	using Result = std::invoke_result_t<F, Args...>;

	if(glfwGetKey(window, key) != action) {
		if constexpr (std::is_void_v<Result>){
			return false;
		}
		else if constexpr (!std::is_void_v<Result>) {
			return std::optional<Result>{};
		}
	}

	if constexpr (std::is_void_v<Result>) {
			std::invoke(
				std::forward<F>(func),
				std::forward<Args>(args)...
			);
			return true;
		}
		else {
			return std::optional<Result>{
				std::in_place,
				std::invoke(
					std::forward<F>(func),
					std::forward<Args>(args)...
				)
			};
		}
}

template<typename F, typename... Args>
	requires std::invocable<F, Args...>
auto ProcessInput(GLFWwindow* window, int key, F&& func,Args&&... args)
{
	return ProcessInput(window, key, GLFW_PRESS, func, args);
}
