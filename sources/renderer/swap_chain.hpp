#ifndef SWAP_CHAIN_HPP
#define SWAP_CHAIN_HPP

#include "device.hpp"
#include <vector>

class SwapChain
{
public:
	SwapChain() = default;

	VkSurfaceFormatKHR getSurfaceFormat();

	bool createSwapChain(Device device,
						 VkSurfaceKHR surface);

	bool createFrameBuffer(uint32_t height, uint32_t width, VkRenderPass renderpass,
						   VkDevice logical_device);

	VkSwapchainKHR *getSwapChainPtr();

	VkFramebuffer getFrameBuffer(int index);

private:
	VkSurfaceFormatKHR m_surface_format;

	VkSwapchainKHR m_swapchain;

	VkImage m_swapchain_images[5];
	VkImageView sc_image_views[5];

	VkFramebuffer m_frame_buffer[5];

	uint32_t swapchain_image_count;
};

#endif // SWAP_CHAIN_HPP