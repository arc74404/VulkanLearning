#include "swap_chain.hpp"
#include <vector>
#include "vk_init.hpp"
#include <iostream>

#define ArraySize(arr) (sizeof((arr)) / sizeof((arr[0])))

#define VK_CHECK(result)                  \
	if (result != VK_SUCCESS)             \
	{                                     \
		std::cout << result << std::endl; \
		__debugbreak();                   \
		return false;                     \
	}

bool SwapChain::createSwapChain(Device device, VkSurfaceKHR surface)
{

	VkPhysicalDevice physical_device = device.getPhysicalDevice();
	VkDevice logical_device = device.getLogicalDevice();

	uint32_t format_count = 0;
	std::vector<VkSurfaceFormatKHR> surface_formats;

	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, 0));
	surface_formats.resize(format_count);
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, surface_formats.data()));

	for (uint32_t i = 0; i < format_count; ++i)
	{
		VkSurfaceFormatKHR format = surface_formats[i];

		if (format.format == VK_FORMAT_R8G8B8A8_SRGB)
		{
			m_surface_format = format;
			break;
		}
	}

	VkSurfaceCapabilitiesKHR surface_caps = {};
	(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_caps));

	uint32_t image_count = 0;
	image_count = surface_caps.minImageCount + 1;

	image_count = image_count > surface_caps.maxImageCount ? image_count - 1 : image_count;

	VkSwapchainCreateInfoKHR swapchain_info = {};
	swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_info.surface = surface;
	swapchain_info.imageFormat = m_surface_format.format;
	swapchain_info.preTransform = surface_caps.currentTransform;
	swapchain_info.imageExtent = surface_caps.currentExtent;
	swapchain_info.minImageCount = image_count;
	swapchain_info.imageArrayLayers = 1;

	VkImageCreateInfo image_info = image_create_info();

	vkCreateImage(logical_device, &image_info, 0, m_swapchain_images.data());

	(vkCreateSwapchainKHR(logical_device, &swapchain_info,
						  0, &m_swapchain));

	uint32_t swapchain_image_count = 0;

	VK_CHECK(vkGetSwapchainImagesKHR(logical_device, m_swapchain,
									 &swapchain_image_count, 0));

	m_swapchain_images.resize(swapchain_image_count);

	VK_CHECK(vkGetSwapchainImagesKHR(logical_device, m_swapchain,
									 &swapchain_image_count, m_swapchain_images.data()));

	// Create the image views
	sc_image_views.resize(swapchain_image_count);

	{
		VkImageViewCreateInfo view_info = {};

		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.format = m_surface_format.format;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.layerCount = 1;
		view_info.subresourceRange.levelCount = 1;

		for (uint32_t i = 0; i < m_swapchain_images.size(); ++i)
		{
			view_info.image = m_swapchain_images[i];
			VK_CHECK(vkCreateImageView(logical_device, &view_info, 0, &sc_image_views[i]));
		}
	}

	return true;
}

bool SwapChain::createFrameBuffer(uint32_t height, uint32_t width, VkRenderPass renderpass,
								  VkDevice logical_device)
{
	VkFramebufferCreateInfo frame_buffer_info = {};
	frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frame_buffer_info.height = height;
	frame_buffer_info.width = width;
	frame_buffer_info.renderPass = renderpass;
	frame_buffer_info.layers = 1;
	frame_buffer_info.attachmentCount = 1;

	for (uint32_t i = 0; i < m_swapchain_images.size(); ++i)
	{
		frame_buffer_info.pAttachments = &sc_image_views[i];
		(vkCreateFramebuffer(logical_device, &frame_buffer_info, 0, &m_frame_buffer[i]));
	}
	return true;
}