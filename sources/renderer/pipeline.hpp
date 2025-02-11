#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "vulkan/vulkan.h"

#include "device.hpp"

class Pipeline
{
public:
	bool createPipeline(Device &device, VkRenderPass &renderpass);

private:
	VkPipelineLayout m_pipeline_layout;

	VkPipeline m_pipeline;
};

#endif // PIPELINE_HPP