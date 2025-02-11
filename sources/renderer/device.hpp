#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <vulkan/vulkan.h>

class Device
{
public:
	Device() = default;

	bool chooseGPU(VkInstance vk_instance, VkSurfaceKHR vk_surface);

	bool createLogicalDevice();

	VkDevice getLogicalDevice();

	VkPhysicalDevice getPhysicalDevice();

private:
	VkPhysicalDevice m_physical_device;

	VkDevice m_logical_device;

	VkQueue m_graphics_queue;

	int m_graphics_index;
};

#endif // DEVICE_HPP