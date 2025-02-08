#ifndef VK_RENDERER_HPP
#define VK_RENDERER_HPP

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <vector>

#define VK_CHECK(result)                  \
	if (result != VK_SUCCESS)             \
	{                                     \
		std::cout << result << std::endl; \
		__debugbreak();                   \
		return false;                     \
	}

static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
	VkDebugUtilsMessageTypeFlagsEXT msg_flags,
	const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
	void *user_data);

struct VkContext
{
	VkInstance instance;
	VkSurfaceFormatKHR surface_format;
	VkSurfaceKHR surface;
	VkDevice device;
	VkSwapchainKHR swapchain;
	VkDebugUtilsMessengerEXT debug_messenger;
	VkQueue graphics_queue;

	VkCommandPool command_pool;

	uint32_t swapchain_image_count;
	std::vector<VkImage> swapchain_images;

	int graphics_index;

	VkPhysicalDevice gpu;
};

bool vk_init(VkContext *vk_context, GLFWwindow *window);

bool vk_render(VkContext *vk_context);

#endif // VK_RENDERER_HPP