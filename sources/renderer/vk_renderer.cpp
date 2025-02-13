#include "vk_renderer.hpp"
#include <vector>
#include "vk_init.hpp"

#include "dds_structs.hpp"

#include "vk_types.hpp"

#define ArraySize(arr) (sizeof((arr)) / sizeof((arr[0])))

// static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
// 	VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
// 	VkDebugUtilsMessageTypeFlagsEXT msg_flags,
// 	const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
// 	void *user_data)
// {
// 	std::cout << "validation error: " << p_callback_data->pMessage << std::endl;
// 	return VK_FALSE;
// };

bool Renderer::createInstance()
{
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Pong";
	app_info.pEngineName = "engine";

	std::vector<const char *> enabled_extensions;

	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	for (uint32_t i = 0; i < glfwExtensionCount; ++i)
	{
		enabled_extensions.push_back(glfwExtensions[i]);
	}
	enabled_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	// validation Layers
	const char *
		validation_layers[] = {
			"VK_LAYER_KHRONOS_validation"};

	VkInstanceCreateInfo instance_info = {};
	instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_info.pApplicationInfo = &app_info;
	instance_info.enabledExtensionCount = enabled_extensions.size();
	instance_info.ppEnabledExtensionNames = enabled_extensions.data();
	instance_info.enabledLayerCount = ArraySize(validation_layers);
	instance_info.ppEnabledLayerNames = validation_layers;

	VK_CHECK(vkCreateInstance(&instance_info, 0, &m_instance));
	return true;
}

bool Renderer::createDebugMessenger()
{
	auto vkCreateDebugUtilsMessengerEXT =
		(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");

	if (vkCreateDebugUtilsMessengerEXT)
	{
		VkDebugUtilsMessengerCreateInfoEXT debug_info = {};
		debug_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debug_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		debug_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

		debug_info.pfnUserCallback = vk_debug_callback;

		vkCreateDebugUtilsMessengerEXT(m_instance, &debug_info, 0, &m_debug_messenger);
	}
	else
	{
		return false;
	}
	return true;
}

bool Renderer::createSurface(GLFWwindow *window)
{
	VK_CHECK(glfwCreateWindowSurface(m_instance, window, NULL, &m_surface));
	return true;
}

bool Renderer::createDevice()
{
	m_device.chooseGPU(m_instance, m_surface);
	m_device.createLogicalDevice();
	return true;
}

// bool Renderer::createSwapChain(uint32_t height, uint32_t width)
// {

// 	return true;
// }

bool Renderer::createRenderPass(VkSurfaceFormatKHR surface_format, VkDevice logical_device)
{
	VkAttachmentDescription attachment = {};
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.format = surface_format.format;

	VkAttachmentReference color_attachmnet_ref = {};
	color_attachmnet_ref.attachment = 0;
	color_attachmnet_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass_description = {};
	subpass_description.colorAttachmentCount = 1;
	subpass_description.pColorAttachments = &color_attachmnet_ref;

	VkAttachmentDescription attachments[] =
		{
			attachment};

	VkRenderPassCreateInfo renderpass_info = {};
	renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpass_info.attachmentCount = ArraySize(attachments);
	renderpass_info.pAttachments = attachments;
	renderpass_info.subpassCount = 1;
	renderpass_info.pSubpasses = &subpass_description;
	VK_CHECK(vkCreateRenderPass(logical_device, &renderpass_info, 0, &m_renderpass));

	return true;
}

bool Renderer::createPipeline()
{
	m_pipeline.createPipeline(m_device, m_renderpass);
	return true;
}

bool Renderer::createCommandPool(VkDevice logical_device, uint32_t graphic_index)
{
	VkCommandPoolCreateInfo command_pool_info = {};
	command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_info.queueFamilyIndex = graphic_index;
	VK_CHECK(vkCreateCommandPool(logical_device, &command_pool_info, 0, &m_command_pool));

	return true;
}

bool Renderer::createSemaphores(VkDevice logical_device)
{
	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VK_CHECK(vkCreateSemaphore(logical_device, &semaphore_info, 0, &m_acuire_semaphore));
	VK_CHECK(vkCreateSemaphore(logical_device, &semaphore_info, 0, &m_submit_semaphore));
	return true;
}

Renderer::Renderer(Window *window)
{
	m_screen_rect = window->getScreenRect();

	createInstance();

	createDebugMessenger();

	createSurface(window->getGlfwWindowPtr());

	createDevice();

	m_swapchin.createSwapChain(m_device, m_surface);

	createRenderPass(m_swapchin.getSurfaceFormat(), m_device.getLogicalDevice());

	m_swapchin.createFrameBuffer(m_screen_rect.extent.height, m_screen_rect.extent.width,
								 m_renderpass, m_device.getLogicalDevice());

	createPipeline();

	createCommandPool(m_device.getLogicalDevice(), m_device.getGraphicIndex());

	createSemaphores(m_device.getLogicalDevice());
}

bool Renderer::render()
{
	uint32_t image_index;
	VK_CHECK(vkAcquireNextImageKHR(m_device.getLogicalDevice(), *(m_swapchin.getSwapChainPtr()),
								   0, m_acuire_semaphore, 0, &image_index));

	VkCommandBuffer cmd;
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandBufferCount = 1;
	alloc_info.commandPool = m_command_pool;

	VK_CHECK(vkAllocateCommandBuffers(m_device.getLogicalDevice(), &alloc_info, &cmd));

	VkCommandBufferBeginInfo begin_info = cmd_begin_info();
	VK_CHECK(vkBeginCommandBuffer(cmd, &begin_info));

	VkClearValue clear_value = {};
	clear_value.color = {1, 1, 0, 1};

	VkRenderPassBeginInfo rp_begin_info = {};
	rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rp_begin_info.renderPass = m_renderpass;
	rp_begin_info.renderArea.extent = m_screen_rect.extent;
	rp_begin_info.framebuffer = m_swapchin.getFrameBuffer(image_index);
	rp_begin_info.pClearValues = &clear_value;
	rp_begin_info.clearValueCount = 1;

	vkCmdBeginRenderPass(cmd, &rp_begin_info, VK_SUBPASS_CONTENTS_INLINE);

	// Rendering Commands
	{
		VkViewport viewport = {};
		viewport.maxDepth = 1.0f;
		viewport.width = m_screen_rect.extent.width;
		viewport.height = m_screen_rect.extent.height;

		VkRect2D scissor = {};
		scissor.extent = m_screen_rect.extent;

		vkCmdSetViewport(cmd, 0, 1, &viewport);
		vkCmdSetScissor(cmd, 0, 1, &scissor);

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.getPipeline());
		vkCmdDraw(cmd, 3, 1, 0, 0);
	}

	vkCmdEndRenderPass(cmd);
	VK_CHECK(vkEndCommandBuffer(cmd));

	VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pWaitDstStageMask = &wait_stage;
	submit_info.pCommandBuffers = &cmd;
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &m_submit_semaphore;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &m_acuire_semaphore;
	vkQueueSubmit(m_device.getGraphicsQueue(), 1, &submit_info, 0);

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.pSwapchains = m_swapchin.getSwapChainPtr();
	present_info.swapchainCount = 1;
	present_info.pImageIndices = &image_index;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &m_submit_semaphore;
	VK_CHECK(vkQueuePresentKHR(m_device.getGraphicsQueue(), &present_info));

	VK_CHECK(vkDeviceWaitIdle(m_device.getLogicalDevice()));

	vkFreeCommandBuffers(m_device.getLogicalDevice(), m_command_pool, 1, &cmd);
	return true;
}
