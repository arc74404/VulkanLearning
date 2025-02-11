#ifndef VK_INIT_HPP
#define VK_INIT_HPP

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

VkCommandBufferBeginInfo cmd_begin_info();

VkImageCreateInfo image_create_info();

std::vector<uint32_t> read_file(const std::string &filename);

#endif // VK_INIT_HPP