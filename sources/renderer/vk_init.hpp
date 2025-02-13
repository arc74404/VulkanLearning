#ifndef VK_INIT_HPP
#define VK_INIT_HPP

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <GLFW/glfw3.h>

class VkContext;

VkCommandBufferBeginInfo cmd_begin_info();

VkImageCreateInfo image_create_info();

bool vk_init(VkContext *vk_context, GLFWwindow *window);

bool vk_render(VkContext *vk_context);

char *read_file(const std::string &filename, uint32_t &size);

#endif // VK_INIT_HPP