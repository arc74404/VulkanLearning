#include "pipeline.hpp"

#include "vk_init.hpp"

#include "device.hpp"

#include <vector>
#include <iostream>

#define VK_CHECK(result)                  \
	if (result != VK_SUCCESS)             \
	{                                     \
		std::cout << result << std::endl; \
		__debugbreak();                   \
		return false;                     \
	}

bool Pipeline::createPipeline(Device &device, VkRenderPass &renderpass)
{
	VkPhysicalDevice physical_device = device.getPhysicalDevice();
	VkDevice logical_device = device.getLogicalDevice();

	VkPipelineLayoutCreateInfo layout_info = {};
	layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	VK_CHECK(vkCreatePipelineLayout(logical_device, &layout_info, 0, &m_pipeline_layout));

	VkShaderModule vertex_shader, fragment_shader;
	// Vertex Shader
	{

		auto code = read_file("C:/Users/arsbo/source/repos/vulkan triangle/assets/shaders/shader.vert.spv");

		size_t code_size = code.size() * sizeof(uint32_t);

		VkShaderModuleCreateInfo shader_module_info = {};
		shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shader_module_info.pCode = code.data();
		shader_module_info.codeSize = code_size;
		VK_CHECK(vkCreateShaderModule(logical_device, &shader_module_info, 0, &vertex_shader));
	}
	// Fragment Shader
	{
		auto code = read_file("C:/Users/arsbo/source/repos/vulkan triangle/assets/shaders/shader.frag.spv");
		size_t code_size = code.size() * sizeof(uint32_t);

		VkShaderModuleCreateInfo shader_module_info = {};
		shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shader_module_info.pCode = code.data();
		shader_module_info.codeSize = code_size;
		VK_CHECK(vkCreateShaderModule(logical_device, &shader_module_info, 0, &fragment_shader));
	}

	VkPipelineMultisampleStateCreateInfo multisample_state = {};
	multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
	input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineShaderStageCreateInfo vert_stage = {};
	vert_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_stage.pName = "main";
	vert_stage.module = vertex_shader;

	VkPipelineShaderStageCreateInfo frag_stage = {};
	frag_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_stage.pName = "main";
	frag_stage.module = fragment_shader;

	std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {vert_stage, frag_stage};

	VkPipelineVertexInputStateCreateInfo vertex_input_state = {};
	vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	VkPipelineColorBlendAttachmentState color_blend_attachment = {};
	color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo color_blend_state = {};
	color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_state.pAttachments = &color_blend_attachment;
	color_blend_state.attachmentCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterization_state = {};
	rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterization_state.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization_state.lineWidth = 1.0f;

	VkRect2D scissor = {};
	VkViewport viewport = {};

	VkPipelineViewportStateCreateInfo viewport_state = {};
	viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state.pViewports = &viewport;
	viewport_state.viewportCount = 1;
	viewport_state.pScissors = &scissor;
	viewport_state.scissorCount = 1;

	std::vector<VkDynamicState> dynamic_states = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR};

	VkPipelineDynamicStateCreateInfo dynamic_state = {};
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.pDynamicStates = dynamic_states.data();
	dynamic_state.dynamicStateCount = dynamic_states.size();

	VkGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.layout = m_pipeline_layout;
	pipeline_info.renderPass = renderpass;
	pipeline_info.pColorBlendState = &color_blend_state;
	pipeline_info.pVertexInputState = &vertex_input_state;
	pipeline_info.pStages = shader_stages.data();
	pipeline_info.stageCount = shader_stages.size();
	pipeline_info.pRasterizationState = &rasterization_state;
	pipeline_info.pViewportState = &viewport_state;
	pipeline_info.pDynamicState = &dynamic_state;
	pipeline_info.pInputAssemblyState = &input_assembly;
	pipeline_info.pMultisampleState = &multisample_state;

	vkCreateGraphicsPipelines(logical_device, 0, 1,
							  &pipeline_info, 0, &m_pipeline);
	return true;
}