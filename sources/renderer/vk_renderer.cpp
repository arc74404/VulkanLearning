#include "vk_renderer.hpp"
#include <vector>

#define ArraySize(arr) (sizeof((arr)) / sizeof((arr[0])))

static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
	VkDebugUtilsMessageTypeFlagsEXT msg_flags,
	const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
	void *user_data)
{
	std::cout << "validation error: " << p_callback_data->pMessage << std::endl;
	return VK_FALSE;
};

bool vk_init(VkContext *vk_context, GLFWwindow *window)
{

	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Pong";
	app_info.pEngineName = "engine";

	const char *extensions[] = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME};

	uint32_t glfw_extension_count = 0;
	const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	// validation Layers
	const char *validation_layers[] = {
		"VK_LAYER_KHRONOS_validation"};

	VkInstanceCreateInfo instance_info = {};
	instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_info.pApplicationInfo = &app_info;
	instance_info.enabledExtensionCount = glfw_extension_count;
	instance_info.ppEnabledExtensionNames = glfw_extensions;
	instance_info.enabledLayerCount = ArraySize(validation_layers);
	instance_info.ppEnabledLayerNames = validation_layers;

	VK_CHECK(vkCreateInstance(&instance_info, 0, &vk_context->instance));

	auto vkCreateDebugUtilsMessengerEXT =
		(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vk_context->instance, "vkCreateDebugUtilsMessengerEXT");

	if (vkCreateDebugUtilsMessengerEXT)
	{
		VkDebugUtilsMessengerCreateInfoEXT debug_info = {};
		debug_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debug_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		debug_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

		debug_info.pfnUserCallback = vk_debug_callback;

		vkCreateDebugUtilsMessengerEXT(vk_context->instance, &debug_info, 0, &vk_context->debug_messenger);
	}
	else
	{
		return false;
	}
	// create Surface
	{
		glfwCreateWindowSurface(vk_context->instance, window, NULL, &vk_context->surface);
	}

	// choose GPU
	{
		vk_context->graphics_index = -1;

		uint32_t gpu_count = 0;

		VkPhysicalDevice gpus[10];

		VK_CHECK(vkEnumeratePhysicalDevices(vk_context->instance,
											&gpu_count, 0));
		VK_CHECK(vkEnumeratePhysicalDevices(vk_context->instance,
											&gpu_count, gpus));

		for (uint32_t i = 0; i < gpu_count; ++i)
		{
			VkPhysicalDevice gpu = gpus[i];

			uint32_t queue_family_count = 0;

			VkQueueFamilyProperties queue_props[10];

			vkGetPhysicalDeviceQueueFamilyProperties(gpu,
													 &queue_family_count,
													 0);

			vkGetPhysicalDeviceQueueFamilyProperties(gpu,
													 &queue_family_count,
													 queue_props);

			for (uint32_t j = 0; j < queue_family_count; ++j)
			{

				if (queue_props[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					VkBool32 surface_support = VK_FALSE;
					VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, j,
																  vk_context->surface, &surface_support));

					if (surface_support)
					{
						vk_context->graphics_index = j;
						vk_context->gpu = gpu;
						break;
					}
				}
			}
		}

		if (vk_context->graphics_index < 0)
		{
			return false;
		}
	}

	// Logical Device
	{
		float queue_priority = 1.f;

		VkDeviceQueueCreateInfo queue_info = {};
		queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info.pQueuePriorities = &queue_priority;
		queue_info.queueFamilyIndex = vk_context->graphics_index;
		queue_info.queueCount = 1;

		char *swapchain_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		VkDeviceCreateInfo device_info = {};
		device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_info.pQueueCreateInfos = &queue_info;
		device_info.queueCreateInfoCount = 1;
		device_info.enabledExtensionCount = ArraySize(swapchain_extensions);
		device_info.ppEnabledExtensionNames = swapchain_extensions;

		VK_CHECK(vkCreateDevice(vk_context->gpu, &device_info,
								0, &vk_context->device));

		vkGetDeviceQueue(vk_context->device, vk_context->graphics_index, 0, &vk_context->graphics_queue);
	}

	// Swap Chain
	{
		uint32_t format_count = 0;
		VkSurfaceFormatKHR surface_formats[10];
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_context->gpu, vk_context->surface, &format_count, 0));
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_context->gpu, vk_context->surface, &format_count, surface_formats));

		for (uint32_t i = 0; i < format_count; ++i)
		{
			VkSurfaceFormatKHR format = surface_formats[i];

			if (format.format == VK_FORMAT_B8G8R8_SRGB)
			{
				vk_context->surface_format = format;
				break;
			}
		}

		VkSurfaceCapabilitiesKHR surface_caps = {};
		VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_context->gpu, vk_context->surface, &surface_caps));

		uint32_t image_count = 0;
		image_count = surface_caps.minImageCount + 1;

		image_count = image_count > surface_caps.maxImageCount ? image_count - 1 : image_count;

		VkSwapchainCreateInfoKHR swapchain_info = {};
		swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchain_info.surface = vk_context->surface;
		swapchain_info.imageFormat = vk_context->surface_format.format;
		swapchain_info.preTransform = surface_caps.currentTransform;
		swapchain_info.imageExtent = surface_caps.currentExtent;
		swapchain_info.minImageCount = image_count;
		swapchain_info.imageArrayLayers = 1;

		VK_CHECK(vkCreateSwapchainKHR(vk_context->device, &swapchain_info,
									  0, &vk_context->swapchain));

		VK_CHECK(vkGetSwapchainImagesKHR(vk_context->device, vk_context->swapchain,
										 &vk_context->swapchain_image_count, 0));
		VK_CHECK(vkGetSwapchainImagesKHR(vk_context->device, vk_context->swapchain,
										 &vk_context->swapchain_image_count, &vk_context->swapchain_images.begin().operator*()));
	}
	// Command Pool
	{
		VkCommandPoolCreateInfo command_pool_info = {};
		command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		command_pool_info.queueFamilyIndex = vk_context->graphics_index;
		VK_CHECK(vkCreateCommandPool(vk_context->device, &command_pool_info, 0, &vk_context->command_pool));
	}

	return true;
}

bool vk_render(VkContext *vk_context)
{
	uint32_t image_index;
	VK_CHECK(vkAcquireNextImageKHR(vk_context->device, vk_context->swapchain, 0, 0, 0, &image_index));

	VkCommandBuffer cmd;
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandBufferCount = 1;
	alloc_info.commandPool = vk_context->command_pool;
	VK_CHECK(vkAllocateCommandBuffers(vk_context->device, &alloc_info, &cmd));

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VK_CHECK(vkBeginCommandBuffer(cmd, &begin_info));
	// Rendering Commands
	{
	}

	VK_CHECK(vkEndCommandBuffer(cmd));

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &cmd;

	vkQueueSubmit(vk_context->graphics_queue, 1, &submit_info, 0);

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.pSwapchains = &vk_context->swapchain;
	present_info.swapchainCount = 1;
	present_info.pImageIndices = &image_index;

	VK_CHECK(vkQueuePresentKHR(vk_context->graphics_queue, &present_info));

	VK_CHECK(vkDeviceWaitIdle(vk_context->device));

	vkFreeCommandBuffers(vk_context->device, vk_context->command_pool, 1, &cmd);
	return true;
}