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

VkCommandBufferAllocateInfo cmd_alloc_info(VkCommandPool pool);

VkSubmitInfo submit_info(VkCommandBuffer *cmd, uint32_t cmdCount = 1);

VkFenceCreateInfo fence_info(VkFenceCreateFlags flags = 0);

#endif // VK_INIT_HPP