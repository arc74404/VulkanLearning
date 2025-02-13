#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include "sources/renderer/vk_renderer.hpp"

#include <thread>
#include <chrono>

#include "sources/renderer/window.hpp"

#define CONTEXT_VERSION

#ifdef CONTEXT_VERSION

GLFWwindow *create_window()
{
	GLFWwindow *window = NULL;

	window = glfwCreateWindow(640, 480, "Triangle", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}

	return window;
}

int run_window(VkContext *vk_context, GLFWwindow *window)
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		if (!vk_render(vk_context))
			return -1;
	}
	return 0;
}

#endif // CONTEXT_VERSION

int main()
{
#ifdef CONTEXT_VERSION

	if (!glfwInit())
	{
		std::cerr << "GLFW initialization failed!" << std::endl;
		return -1;
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow *glf_window = create_window();

	VkContext vk_context;
	vk_context.surface = VK_NULL_HANDLE;

	if (!vk_init(&vk_context, glf_window))
	{
		std::cout << "vk_init returned false\n";
		return -1;
	};

	run_window(&vk_context, glf_window);
#elif  // CONTEXT_VERSION

	Window glfw_window(640u, 480u, "Triangle");
	Renderer renderer(&glfw_window);
	glfw_window.run(&renderer);
#endif // CONTEXT_VERSION

	vkDestroySurfaceKHR(vk_context.instance, vk_context.surface, 0);
	return 0;
}