#ifndef VK_RENDERER_HPP
#define VK_RENDERER_HPP

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <vector>

#include "window.hpp"
#include "device.hpp"

#include "swap_chain.hpp"
#include "pipeline.hpp"

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

class Renderer
{
public:
	Renderer(Window *window);

	bool render();

private:
	bool createInstance();

	bool createDebugMessenger();

	bool createSurface(GLFWwindow *window);

	bool createDevice();

	bool createSwapChain(uint32_t height, uint32_t width);

	bool createRenderPass(VkSurfaceFormatKHR surface_format, VkDevice logical_device);

	bool createFrameBuffer();

	bool createPipeline();

	bool createCommandPool(VkDevice logical_device, uint32_t graphic_index);

	bool createSemaphores(VkDevice logical_device);

	VkInstance m_instance;

	VkDebugUtilsMessengerEXT m_debug_messenger;

	VkSurfaceKHR m_surface;

	SwapChain m_swapchin;

	Device m_device;

	VkRenderPass m_renderpass;

	Pipeline m_pipeline;

	VkCommandPool m_command_pool;

	VkSemaphore m_submit_semaphore;
	VkSemaphore m_acuire_semaphore;

	VkRect2D m_screen_rect;
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

	VkCommandPool command_pool;

	VkSemaphore submit_semaphore;
	VkSemaphore acuire_semaphore;

	uint32_t swapchain_image_count;

	VkImage swapchain_images[5];
	VkFramebuffer frame_buffer[5];
	VkImageView sc_image_views[5];

	VkRect2D screen_rect;

	VkRenderPass renderpass;

	int graphics_index;

	VkPhysicalDevice gpu;
};

bool vk_init(VkContext *vk_context, GLFWwindow *window);

bool vk_render(VkContext *vk_context);

#endif // VK_RENDERER_HPP