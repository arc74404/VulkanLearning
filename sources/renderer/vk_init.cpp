#include "vk_init.hpp"
#include <fstream>
#include <iostream>
#include "vk_types.hpp"
#include "dds_structs.hpp"

#include "vk_renderer.hpp"
#define ArraySize(arr) (sizeof((arr)) / sizeof((arr[0])))

VkCommandBufferBeginInfo cmd_begin_info()
{
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	return begin_info;
}

VkImageCreateInfo image_create_info()
{
	VkImageCreateInfo image_info;
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.pNext = nullptr;
	image_info.flags = 0;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.format = VK_FORMAT_R8G8B8A8_SRGB;
	image_info.extent.width = 800;
	image_info.extent.height = 600;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; // Add sampled bit if you want to sample from the image in a shader
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.queueFamilyIndexCount = 0;
	image_info.pQueueFamilyIndices = nullptr;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	return image_info;
}

char *read_file(const std::string &filename, uint32_t &size)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file");
	}

	// Получаем размер файла
	size_t fileSize = file.tellg();
	file.seekg(0);

	// Выделяем память для буфера, который будет содержать файл
	char *buffer = new char[fileSize];

	size = fileSize;

	// Читаем данные из файла в буфер
	file.read(buffer, fileSize);
	file.close();

	return buffer; // возвращаем указатель на содержимое файла
}

bool vk_init(VkContext *vk_context, GLFWwindow *window)
{
	// Screen Rect
	{
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		vk_context->screen_rect.extent.height = height;
		vk_context->screen_rect.extent.width = width;
	}
	/////////////////////////// create instance
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

	VK_CHECK(vkCreateInstance(&instance_info, 0, &vk_context->instance));
	///////////////////////////

	/////////////////////////// create debug messenger

	auto vkCreateDebugUtilsMessengerEXT =
		(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vk_context->instance, "vkCreateDebugUtilsMessengerEXT");

	if (vkCreateDebugUtilsMessengerEXT)
	{
		VkDebugUtilsMessengerCreateInfoEXT debug_info = {};
		debug_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debug_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		debug_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

		debug_info.pfnUserCallback = vk_debug_callback;

		vkCreateDebugUtilsMessengerEXT(vk_context->instance, &debug_info, 0, &vk_context->debug_messenger);
	}
	else
	{
		return false;
	}
	///////////////////////////
	// create Surface
	{
		VK_CHECK(glfwCreateWindowSurface(vk_context->instance, window, NULL, &vk_context->surface));
	}

	// choose GPU
	{
		vk_context->graphics_index = -1;

		uint32_t gpu_count = 0;

		VkPhysicalDevice gpus[10];

		VK_CHECK(vkEnumeratePhysicalDevices(vk_context->instance,
											&gpu_count, 0));
		VK_CHECK(vkEnumeratePhysicalDevices(vk_context->instance,
											&gpu_count, gpus));

		for (uint32_t i = 0; i < gpu_count; ++i)
		{
			VkPhysicalDevice gpu = gpus[i];

			uint32_t queue_family_count = 0;

			VkQueueFamilyProperties queue_props[10];

			vkGetPhysicalDeviceQueueFamilyProperties(gpu,
													 &queue_family_count,
													 0);

			vkGetPhysicalDeviceQueueFamilyProperties(gpu,
													 &queue_family_count,
													 queue_props);

			for (uint32_t j = 0; j < queue_family_count; ++j)
			{

				if (queue_props[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					VkBool32 surface_support = VK_FALSE;
					VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, j,
																  vk_context->surface, &surface_support));

					if (surface_support)
					{
						vk_context->graphics_index = j;
						vk_context->gpu = gpu;
						break;
					}
				}
			}
		}

		if (vk_context->graphics_index < 0)
		{
			return false;
		}
	}

	// Logical Device
	{
		float queue_priority = 1.f;

		VkDeviceQueueCreateInfo queue_info = {};
		queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info.pQueuePriorities = &queue_priority;
		queue_info.queueFamilyIndex = vk_context->graphics_index;
		queue_info.queueCount = 1;

		const char *swapchain_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		VkDeviceCreateInfo device_info = {};
		device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_info.pQueueCreateInfos = &queue_info;
		device_info.queueCreateInfoCount = 1;
		device_info.enabledExtensionCount = ArraySize(swapchain_extensions);
		device_info.ppEnabledExtensionNames = swapchain_extensions;

		VK_CHECK(vkCreateDevice(vk_context->gpu, &device_info,
								0, &vk_context->device));

		vkGetDeviceQueue(vk_context->device, vk_context->graphics_index, 0, &vk_context->graphics_queue);
	}

	// Swap Chain
	{
		uint32_t format_count = 0;
		VkSurfaceFormatKHR surface_formats[10];
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_context->gpu, vk_context->surface, &format_count, 0));
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_context->gpu, vk_context->surface, &format_count, surface_formats));

		for (uint32_t i = 0; i < format_count; ++i)
		{
			VkSurfaceFormatKHR format = surface_formats[i];

			if (format.format == VK_FORMAT_R8G8B8A8_SRGB)
			{
				vk_context->surface_format = format;
				break;
			}
		}

		VkSurfaceCapabilitiesKHR surface_caps = {};
		VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_context->gpu, vk_context->surface, &surface_caps));

		uint32_t image_count = 0;
		image_count = surface_caps.minImageCount + 1;

		image_count = image_count > surface_caps.maxImageCount ? image_count - 1 : image_count;

		VkSwapchainCreateInfoKHR swapchain_info = {};
		swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchain_info.surface = vk_context->surface;
		swapchain_info.imageFormat = vk_context->surface_format.format;
		swapchain_info.preTransform = surface_caps.currentTransform;
		swapchain_info.imageExtent = surface_caps.currentExtent;
		swapchain_info.minImageCount = image_count;
		swapchain_info.imageArrayLayers = 1;

		VkImageCreateInfo image_info = image_create_info();

		vkCreateImage(vk_context->device, &image_info, 0, vk_context->swapchain_images);

		VK_CHECK(vkCreateSwapchainKHR(vk_context->device, &swapchain_info,
									  0, &vk_context->swapchain));

		VK_CHECK(vkGetSwapchainImagesKHR(vk_context->device, vk_context->swapchain,
										 &vk_context->swapchain_image_count, 0));
		VK_CHECK(vkGetSwapchainImagesKHR(vk_context->device, vk_context->swapchain,
										 &vk_context->swapchain_image_count, vk_context->swapchain_images));

		// Create the image views
		{
			VkImageViewCreateInfo view_info = {};

			view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_info.format = vk_context->surface_format.format;
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			view_info.subresourceRange.layerCount = 1;
			view_info.subresourceRange.levelCount = 1;

			for (uint32_t i = 0; i < vk_context->swapchain_image_count; ++i)
			{
				view_info.image = vk_context->swapchain_images[i];
				VK_CHECK(vkCreateImageView(vk_context->device, &view_info, 0, &vk_context->sc_image_views[i]));
			}
		}
	}

	// Render Path
	{
		VkAttachmentDescription attachment = {};
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.format = vk_context->surface_format.format;

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
		VK_CHECK(vkCreateRenderPass(vk_context->device, &renderpass_info, 0, &vk_context->renderpass));
	}
	// Frame Buffer
	{
		VkFramebufferCreateInfo frame_buffer_info = {};
		frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frame_buffer_info.height = vk_context->screen_rect.extent.height;
		frame_buffer_info.width = vk_context->screen_rect.extent.width;
		frame_buffer_info.renderPass = vk_context->renderpass;
		frame_buffer_info.layers = 1;
		frame_buffer_info.attachmentCount = 1;

		for (uint32_t i = 0; i < vk_context->swapchain_image_count; ++i)
		{
			frame_buffer_info.pAttachments = &vk_context->sc_image_views[i];
			VK_CHECK(vkCreateFramebuffer(vk_context->device, &frame_buffer_info, 0, &vk_context->frame_buffer[i]));
		}
	}
	// Pipeline Layot
	{
		VkPipelineLayoutCreateInfo layout_info = {};
		layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		VK_CHECK(vkCreatePipelineLayout(vk_context->device, &layout_info, 0, &vk_context->pipeline_layout));
	}

	// Pipeline
	{
		VkShaderModule vertex_shader, fragment_shader;
		// Vertex Shader
		{
			uint32_t size = 0;
			uint32_t *vertexCode = (uint32_t *)read_file("C:/Users/arsbo/source/repos/vulkan triangle/assets/shaders/shader.vert.spv", size);

			VkShaderModuleCreateInfo shader_module_info = {};
			shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shader_module_info.pCode = vertexCode;
			shader_module_info.codeSize = size;
			VK_CHECK(vkCreateShaderModule(vk_context->device, &shader_module_info, 0, &vertex_shader));
		}
		// Fragment Shader
		{
			uint32_t size = 0;
			uint32_t *fragmentCode = (uint32_t *)read_file("C:/Users/arsbo/source/repos/vulkan triangle/assets/shaders/shader.frag.spv", size);

			VkShaderModuleCreateInfo shader_module_info = {};
			shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shader_module_info.pCode = fragmentCode;
			shader_module_info.codeSize = size;
			VK_CHECK(vkCreateShaderModule(vk_context->device, &shader_module_info, 0, &fragment_shader));
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

		VkPipelineShaderStageCreateInfo shader_stages[] = {vert_stage, frag_stage};

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

		VkDynamicState dynamic_states[]{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR};

		VkPipelineDynamicStateCreateInfo dynamic_state = {};
		dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state.pDynamicStates = dynamic_states;
		dynamic_state.dynamicStateCount = ArraySize(dynamic_states);

		VkGraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.layout = vk_context->pipeline_layout;
		pipeline_info.renderPass = vk_context->renderpass;
		pipeline_info.pColorBlendState = &color_blend_state;
		pipeline_info.pVertexInputState = &vertex_input_state;
		pipeline_info.pStages = shader_stages;
		pipeline_info.stageCount = ArraySize(shader_stages);
		pipeline_info.pRasterizationState = &rasterization_state;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pDynamicState = &dynamic_state;
		pipeline_info.pInputAssemblyState = &input_assembly;
		pipeline_info.pMultisampleState = &multisample_state;

		vkCreateGraphicsPipelines(vk_context->device, 0, 1,
								  &pipeline_info, 0, &vk_context->pipeline);
	}
	// Command Pool
	{
		VkCommandPoolCreateInfo command_pool_info = {};
		command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		command_pool_info.queueFamilyIndex = vk_context->graphics_index;
		VK_CHECK(vkCreateCommandPool(vk_context->device, &command_pool_info, 0, &vk_context->command_pool));
	}

	// Sync Objects
	{
		VkSemaphoreCreateInfo semaphore_info = {};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VK_CHECK(vkCreateSemaphore(vk_context->device, &semaphore_info, 0, &vk_context->acuire_semaphore));
		VK_CHECK(vkCreateSemaphore(vk_context->device, &semaphore_info, 0, &vk_context->submit_semaphore));
	}

	// Create Image
	{
		uint32_t file_size = 0;
		auto data = (DDSFile *)(read_file("C:/Users/arsbo/source/repos/vulkan triangle/assets/textures/human.dds", file_size));

		VkImageCreateInfo image_info = {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
		image_info.extent = {data->header.Width, data->header.Height, 1};
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		// image_info.initialLayout =VK_IMAGE_LAYOUT_UNDEFINED;

		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(vk_context->device, vk_context->image.image, &memRequirements);

		VkPhysicalDeviceMemoryProperties gpuMemProps;
		vkGetPhysicalDeviceMemoryProperties(vk_context->gpu, &gpuMemProps);

		for (uint32_t i = 0; i < gpuMemProps.memoryTypeCount; i++)
		{
			if (memRequirements.memoryTypeBits & (1 << i) &&
				(gpuMemProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			{
				alloc_info.memoryTypeIndex = i;
			}
		}

		alloc_info.allocationSize = data->header.Height * data->header.Width * 4;

		// VK_CHECK(vkCreateImage(vk_context->device, &image_info, 0, &vk_context->image.image));
		VK_CHECK(vkAllocateMemory(vk_context->device, &alloc_info, 0, &vk_context->image.memory));
		VK_CHECK(vkBindImageMemory(vk_context->device, vk_context->image.image, vk_context->image.memory, 0));
	}

	return true;
}

bool vk_render(VkContext *vk_context)
{
	uint32_t image_index;
	VK_CHECK(vkAcquireNextImageKHR(vk_context->device, vk_context->swapchain,
								   0, vk_context->acuire_semaphore, 0, &image_index));

	VkCommandBuffer cmd;
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandBufferCount = 1;
	alloc_info.commandPool = vk_context->command_pool;
	VK_CHECK(vkAllocateCommandBuffers(vk_context->device, &alloc_info, &cmd));

	VkCommandBufferBeginInfo begin_info = cmd_begin_info();
	VK_CHECK(vkBeginCommandBuffer(cmd, &begin_info));

	VkClearValue clear_value = {};
	clear_value.color = {1, 1, 0, 1};

	VkRenderPassBeginInfo rp_begin_info = {};
	rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rp_begin_info.renderPass = vk_context->renderpass;
	rp_begin_info.renderArea.extent = vk_context->screen_rect.extent;
	rp_begin_info.framebuffer = vk_context->frame_buffer[image_index];
	rp_begin_info.pClearValues = &clear_value;
	rp_begin_info.clearValueCount = 1;

	vkCmdBeginRenderPass(cmd, &rp_begin_info, VK_SUBPASS_CONTENTS_INLINE);
	// Rendering Commands
	{
		VkViewport viewport = {};
		viewport.maxDepth = 1.0f;
		viewport.width = vk_context->screen_rect.extent.width;
		viewport.height = vk_context->screen_rect.extent.height;

		VkRect2D scissor = {};
		scissor.extent = vk_context->screen_rect.extent;

		vkCmdSetViewport(cmd, 0, 1, &viewport);
		vkCmdSetScissor(cmd, 0, 1, &scissor);

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_context->pipeline);
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
	submit_info.pSignalSemaphores = &vk_context->submit_semaphore;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &vk_context->acuire_semaphore;
	vkQueueSubmit(vk_context->graphics_queue, 1, &submit_info, 0);

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.pSwapchains = &vk_context->swapchain;
	present_info.swapchainCount = 1;
	present_info.pImageIndices = &image_index;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &vk_context->submit_semaphore;
	VK_CHECK(vkQueuePresentKHR(vk_context->graphics_queue, &present_info));

	VK_CHECK(vkDeviceWaitIdle(vk_context->device));

	vkFreeCommandBuffers(vk_context->device, vk_context->command_pool, 1, &cmd);
	return true;
}