#include "device.hpp"

#include <iostream>

#define ArraySize(arr) (sizeof((arr)) / sizeof((arr[0])))

#define VK_CHECK(result)                  \
	if (result != VK_SUCCESS)             \
	{                                     \
		std::cout << result << std::endl; \
		__debugbreak();                   \
		return false;                     \
	}

VkDevice Device::getLogicalDevice()
{
	return m_logical_device;
}

VkPhysicalDevice Device::getPhysicalDevice()
{
	return m_physical_device;
}

bool Device::chooseGPU(VkInstance vk_instance, VkSurfaceKHR vk_surface)
{
	m_graphics_index = -1;

	uint32_t gpu_count = 0;

	VkPhysicalDevice gpus[10];

	VK_CHECK(vkEnumeratePhysicalDevices(vk_instance,
										&gpu_count, 0));
	VK_CHECK(vkEnumeratePhysicalDevices(vk_instance,
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
															  vk_surface, &surface_support));

				if (surface_support)
				{
					m_graphics_index = j;
					m_physical_device = gpu;
					break;
				}
			}
		}
	}

	if (m_graphics_index < 0)
	{
		return false;
	}
	return true;
}

bool Device::createLogicalDevice()
{
	float queue_priority = 1.f;

	VkDeviceQueueCreateInfo queue_info = {};
	queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_info.pQueuePriorities = &queue_priority;
	queue_info.queueFamilyIndex = m_graphics_index;
	queue_info.queueCount = 1;

	const char *swapchain_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	VkDeviceCreateInfo device_info = {};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pQueueCreateInfos = &queue_info;
	device_info.queueCreateInfoCount = 1;
	device_info.enabledExtensionCount = ArraySize(swapchain_extensions);
	device_info.ppEnabledExtensionNames = swapchain_extensions;

	VK_CHECK(vkCreateDevice(m_physical_device, &device_info,
							0, &m_logical_device));

	vkGetDeviceQueue(m_logical_device, m_graphics_index, 0, &m_graphics_queue);
	return true;
}