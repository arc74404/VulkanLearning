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

void run_window(VkContext *vk_context, GLFWwindow *window)
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		vk_render(vk_context);
	}
}

int main()
{
	GLFWwindow *glf_window = create_window();

	VkContext vk_context;

	vk_init(&vk_context, glf_window);

	run_window(&vk_context, glf_window);

	glfwDestroyWindow(glf_window);
	glfwTerminate();

	return 0;
}