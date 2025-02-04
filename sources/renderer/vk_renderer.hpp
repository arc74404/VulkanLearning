#ifndef VK_RENDERER_HPP
#define VK_RENDERER_HPP

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#define VK_CHECK(result)                  \
	if (result != VK_SUCCESS)             \
	{                                     \
		std::cout << result << std::endl; \
		__debugbreak();                   \
		return false;                     \
	}

struct VkContext
{
	VkInstance instance;
	VkSurfaceKHR surface;
};

bool vk_init(VkContext *vk_context, GLFWwindow *window);

#endif // VK_RENDERER_HPP