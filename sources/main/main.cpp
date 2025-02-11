#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include "sources/renderer/vk_renderer.hpp"

#include <thread>
#include <chrono>

#include "sources/renderer/window.hpp"

int main()
{

	VkContext vk_context;
	vk_context.surface = VK_NULL_HANDLE;

	Window glfw_window(640u, 480u, "Triangle");
	Renderer renderer(&glfw_window);

	glfw_window.run(&renderer);

	return 0;
}