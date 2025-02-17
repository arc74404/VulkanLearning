#ifndef VK_TYPES_HPP
#define VK_TYPES_HPP

#include <vulkan/vulkan.hpp>

struct Image
{
	VkImage image;
	VkDeviceMemory memory;
	VkImageView view;
};

struct Buffer
{
	VkBuffer buffer;
	VkDeviceMemory memory;
	uint32_t size;
	void *data;
};

#endif // VK_TYPES_HPP