#ifndef SWAP_CHAIN_HPP
#define SWAP_CHAIN_HPP

#include "device.hpp"
#include <vector>

class SwapChain
{
public:
	SwapChain() = default;

	bool createSwapChain(Device device,
						 VkSurfaceKHR surface);

	bool createFrameBuffer(uint32_t height, uint32_t width, VkRenderPass renderpass,
						   VkDevice logical_device);

private:
	VkSurfaceFormatKHR m_surface_format;

	VkSwapchainKHR m_swapchain;

	std::vector<VkImage> m_swapchain_images;
	std::vector<VkImageView> sc_image_views;

	std::vector<VkFramebuffer> m_frame_buffer;
};

#endif // SWAP_CHAIN_HPP