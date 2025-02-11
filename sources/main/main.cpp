#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include "sources/renderer/vk_renderer.hpp"

#include <thread>
#include <chrono>

#include "sources/renderer/window.hpp"

// GLFWwindow *create_window()
// {
// 	GLFWwindow *window = NULL;

// 	window = glfwCreateWindow(640, 480, "Triangle", nullptr, nullptr);

// 	if (window == nullptr)
// 	{
// 		std::cerr << "Failed to create GLFW window" << std::endl;
// 		glfwTerminate();
// 		return false;
// 	}

// 	return window;
// }

// int run_window(VkContext *vk_context, GLFWwindow *window)
// {
// 	while (!glfwWindowShouldClose(window))
// 	{
// 		glfwPollEvents();
// 		if (!vk_render(vk_context))
// 			return -1;
// 	}
// 	return 0;
// }

int main()
{
	// if (!glfwInit())
	// {
	// 	std::cerr << "GLFW initialization failed!" << std::endl;
	// 	return false;
	// }
	// glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// GLFWwindow *glf_window = create_window();
	// glf_window->context.client = GLFW_NO_API;
	// glf_window->cinew;

	VkContext vk_context;
	vk_context.surface = VK_NULL_HANDLE;

	Window glfw_window(640u, 480u, "Triangle");

	if (!vk_init(&vk_context, glfw_window.getGlfwWindowPtr()))
	{
		std::cout << "vk_init returned false\n";
		return -1;
	};

	glfw_window.run(&vk_context);

	// run_window(&vk_context, glfw_window);

	vkDestroySurfaceKHR(vk_context.instance, vk_context.surface, 0);

	return 0;
}