#ifndef VK_RENDERER_HPP
#define VK_RENDERER_HPP

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <vector>

#include "vk_types.hpp"

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
	void *user_data)
{
	std::cout << "validation error: " << p_callback_data->pMessage << std::endl;
	return VK_FALSE;
};

struct VkContext
{
	VkInstance instance;
	VkSurfaceFormatKHR surface_format;
	VkSurfaceKHR surface;
	VkDevice device;
	VkSwapchainKHR swapchain;
	VkDebugUtilsMessengerEXT debug_messenger;
	VkQueue graphics_queue;
	VkPipeline pipeline;
	VkPipelineLayout pipeline_layout;
	Image image;
	Buffer staging_buffer;

	VkCommandPool command_pool;

	VkSemaphore submit_semaphore;
	VkSemaphore acuire_semaphore;

	VkCommandBuffer cmd;

	uint32_t swapchain_image_count;

	VkImage swapchain_images[5];
	VkFramebuffer frame_buffer[5];
	VkImageView sc_image_views[5];

	VkRect2D screen_rect;

	VkRenderPass renderpass;

	int graphics_index;

	VkPhysicalDevice gpu;

	VkSampler sampler;

	VkDescriptorSet desc_set;
	VkDescriptorPool desc_pool;
	VkDescriptorSetLayout set_layout;

	VkFence imgAvailableFence;
};

bool vk_init(VkContext *vk_context, GLFWwindow *window);

bool vk_render(VkContext *vk_context);

#endif // VK_RENDERER_HPP