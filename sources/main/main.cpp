#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include "sources/renderer/vk_renderer.hpp"

GLFWwindow *create_window()
{
	GLFWwindow *window = NULL;

	if (!glfwInit())
	{
		std::cerr << "GLFW initialization failed!" << std::endl;
		return false;
	}

	window = glfwCreateWindow(640, 480, "Triangle", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	return window;
}

void run_window(GLFWwindow *window)
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}
}

int main()
{
	GLFWwindow *glf_window = create_window();

	VkContext vk_context;

	vk_init(&vk_context, glf_window);

	run_window(glf_window);

	glfwDestroyWindow(glf_window);
	glfwTerminate();

	return 0;
}