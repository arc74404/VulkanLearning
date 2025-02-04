#include "vk_renderer.hpp"

bool vk_init(VkContext *vk_context, GLFWwindow *window)
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Pong";
	appInfo.pEngineName = "engine";

	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;

	VK_CHECK(vkCreateInstance(&instanceInfo, 0, &vk_context->instance));

	glfwCreateWindowSurface(vk_context->instance, window, NULL, &vk_context->surface);

	return true;
}