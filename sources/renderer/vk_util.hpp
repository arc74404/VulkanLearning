#ifndef VK_UTIL_HPP
#define VK_UTIL_HPP

// #include <vulkan/vulkan.h>
// #include "vk_types.hpp"

// #define VK_CHECK(result)                  \
// 	if (result != VK_SUCCESS)             \
// 	{                                     \
// 		std::cout << result << std::endl; \
// 		__debugbreak();                   \
// 		return false;                     \
// 	}

// uint32_t vk_get_memory_type_index(
// 	VkPhysicalDevice gpu,
// 	VkMemoryRequirements memRequirements,
// 	VkMemoryPropertyFlags memProps)
// {
// 	uint32_t typeIdx = INVALID_IDX;

// 	VkPhysicalDeviceMemoryProperties gpuMemProps;
// 	vkGetPhysicalDeviceMemoryProperties(gpu, &gpuMemProps);

// 	VkMemoryAllocateInfo allocInfo = {};
// 	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
// 	allocInfo.allocationSize = memRequirements.size;
// 	for (uint32_t i = 0; i < gpuMemProps.memoryTypeCount; i++)
// 	{
// 		if (memRequirements.memoryTypeBits & (1 << i) &&
// 			(gpuMemProps.memoryTypes[i].propertyFlags & memProps) == memProps)
// 		{
// 			typeIdx = i;
// 			break;
// 		}
// 	}

// 	CAKEZ_ASSERT(typeIdx != INVALID_IDX, "Failed to fine proper type Index for Memory Properties: %d", memProps);

// 	return typeIdx;
// }

// Buffer vk_allocate_buffer(
// 	VkDevice device,
// 	VkPhysicalDevice gpu,
// 	uint32_t size,
// 	VkBufferUsageFlags bufferUsage,
// 	VkMemoryPropertyFlags memProps)
// {
// 	Buffer buffer = {};
// 	buffer.size = size;

// 	VkBufferCreateInfo bufferInfo = {};
// 	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
// 	bufferInfo.usage = bufferUsage;
// 	bufferInfo.size = size;
// 	(vkCreateBuffer(device, &bufferInfo, 0, &buffer.buffer));

// 	VkMemoryRequirements memRequirements;
// 	vkGetBufferMemoryRequirements(device, buffer.buffer, &memRequirements);

// 	VkMemoryAllocateInfo allocInfo = {};
// 	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
// 	allocInfo.allocationSize = buffer.size;
// 	allocInfo.memoryTypeIndex = vk_get_memory_type_index(gpu, memRequirements, memProps);

// 	VK_CHECK(vkAllocateMemory(device, &allocInfo, 0, &buffer.memory));

// 	// Only map memory we can actually write to from the CPU
// 	if (memProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
// 	{
// 		VK_CHECK(vkMapMemory(device, buffer.memory, 0, MB(1), 0, &buffer.data));
// 	}

// 	VK_CHECK(vkBindBufferMemory(device, buffer.buffer, buffer.memory, 0));

// 	return buffer;
// }

#endif // VK_UTIL_HPP