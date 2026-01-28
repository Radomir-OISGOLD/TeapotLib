#include "Teapot/pipeline.hpp"
#include "Teapot/descriptors.hpp"
#include "Teapot/vertex.hpp"
#include "Teapot/device.hpp"
#include "Teapot/renderpass.hpp"
#include "Teapot/swapchain.hpp"
#include "Teapot/shader.hpp"

namespace Teapot
{
	Pipeline::Pipeline(Device* p_device, RenderPass* p_render_pass, Swapchain* p_swapchain, Shader& sh_vert, Shader& sh_frag)
		: p_device(p_device)
	{
		vk::PipelineShaderStageCreateInfo vert_stage_info;
		vert_stage_info.stage = vk::ShaderStageFlagBits::eVertex;
		vert_stage_info.module = sh_vert.handle;
		vert_stage_info.pName = "main";
	
		vk::PipelineShaderStageCreateInfo frag_stage_info;
		frag_stage_info.stage = vk::ShaderStageFlagBits::eFragment;
		frag_stage_info.module = sh_frag.handle;
		frag_stage_info.pName = "main";
	
		vk::PipelineShaderStageCreateInfo shader_stages[] = { vert_stage_info, frag_stage_info };

		vk::PipelineVertexInputStateCreateInfo vertex_input_info;
    	vertex_input_info.vertexBindingDescriptionCount = 0;
    	vertex_input_info.vertexAttributeDescriptionCount = 0;

    	vk::PipelineInputAssemblyStateCreateInfo input_assembly;
    	input_assembly.topology = vk::PrimitiveTopology::eTriangleList;
    	input_assembly.primitiveRestartEnable = false;

		vk::Viewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)p_swapchain->vkb_swapchain.extent.width;
		viewport.height = (float)p_swapchain->vkb_swapchain.extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vk::Rect2D scissor;
		scissor.offset = vk::Offset2D{ 0, 0 };
		scissor.extent = p_swapchain->vkb_swapchain.extent;

		vk::PipelineViewportStateCreateInfo viewport_state;
    	viewport_state.viewportCount = 1;
    	viewport_state.pViewports = &viewport;
    	viewport_state.scissorCount = 1;
    	viewport_state.pScissors = &scissor;

		vk::PipelineRasterizationStateCreateInfo rasterizer;
    	rasterizer.depthClampEnable = false;
    	rasterizer.rasterizerDiscardEnable = false;
    	rasterizer.polygonMode = vk::PolygonMode::eFill;
    	rasterizer.lineWidth = 1.0f;
    	rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    	rasterizer.frontFace = vk::FrontFace::eClockwise;
    	rasterizer.depthBiasEnable = false;

		vk::PipelineMultisampleStateCreateInfo multisampling;
    	multisampling.sampleShadingEnable = false;
    	multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

		vk::PipelineColorBlendAttachmentState colorBlendAttachment;
		colorBlendAttachment.colorWriteMask =
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = false;

		vk::PipelineColorBlendStateCreateInfo color_blending;
		color_blending.logicOpEnable = false;
		color_blending.logicOp = vk::LogicOp::eCopy;
		color_blending.attachmentCount = 1;
		color_blending.pAttachments = &colorBlendAttachment;
		color_blending.blendConstants[0] = 0.0f;
		color_blending.blendConstants[1] = 0.0f;
		color_blending.blendConstants[2] = 0.0f;
		color_blending.blendConstants[3] = 0.0f;

		vk::PipelineLayoutCreateInfo pipeline_layout_info;
		pipeline_layout_info.setLayoutCount = 0;
		pipeline_layout_info.pushConstantRangeCount = 0;

		layout = p_device->device.createPipelineLayout(pipeline_layout_info);

		vec<vk::DynamicState> dynamic_states = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };

		vk::PipelineDynamicStateCreateInfo dynamic_info;
		dynamic_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
		dynamic_info.pDynamicStates = dynamic_states.data();
	
		vk::GraphicsPipelineCreateInfo pipeline_info;
		pipeline_info.stageCount = 2;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &input_assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multisampling;
		pipeline_info.pColorBlendState = &color_blending;
		pipeline_info.pDynamicState = &dynamic_info;
		pipeline_info.layout = layout;
		pipeline_info.renderPass = p_render_pass->handle;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

		handle = p_device->device.createGraphicsPipeline(VK_NULL_HANDLE, pipeline_info).value;
	}

	Pipeline::Pipeline(
		Device* p_device,
		RenderPass* p_render_pass,
		Swapchain* p_swapchain,
		Shader& sh_vert,
		Shader& sh_frag,
		DescriptorSetLayout* desc_layout,
		bool enable_vertex_input
	) : p_device(p_device)
	{
		vk::PipelineShaderStageCreateInfo vert_stage_info;
		vert_stage_info.stage = vk::ShaderStageFlagBits::eVertex;
		vert_stage_info.module = sh_vert.handle;
		vert_stage_info.pName = "main";

		vk::PipelineShaderStageCreateInfo frag_stage_info;
		frag_stage_info.stage = vk::ShaderStageFlagBits::eFragment;
		frag_stage_info.module = sh_frag.handle;
		frag_stage_info.pName = "main";

		vk::PipelineShaderStageCreateInfo shader_stages[] = { vert_stage_info, frag_stage_info };

		// Vertex input state (optionally enabled for UI)
		vk::VertexInputBindingDescription binding_desc;
		auto attr_descs = UIVertex::getAttributeDescriptions();

		vk::PipelineVertexInputStateCreateInfo vertex_input_info;

		if (enable_vertex_input)
		{
			binding_desc = UIVertex::getBindingDescription();
			vertex_input_info.vertexBindingDescriptionCount = 1;
			vertex_input_info.pVertexBindingDescriptions = &binding_desc;
			vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attr_descs.size());
			vertex_input_info.pVertexAttributeDescriptions = attr_descs.data();
		}
		else
		{
			vertex_input_info.vertexBindingDescriptionCount = 0;
			vertex_input_info.vertexAttributeDescriptionCount = 0;
		}

		vk::PipelineInputAssemblyStateCreateInfo input_assembly;
		input_assembly.topology = vk::PrimitiveTopology::eTriangleList;
		input_assembly.primitiveRestartEnable = VK_FALSE;

		vk::Viewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)p_swapchain->vkb_swapchain.extent.width;
		viewport.height = (float)p_swapchain->vkb_swapchain.extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vk::Rect2D scissor;
		scissor.offset = vk::Offset2D{ 0, 0 };
		scissor.extent = p_swapchain->vkb_swapchain.extent;

		vk::PipelineViewportStateCreateInfo viewport_state;
		viewport_state.viewportCount = 1;
		viewport_state.pViewports = &viewport;
		viewport_state.scissorCount = 1;
		viewport_state.pScissors = &scissor;

		vk::PipelineRasterizationStateCreateInfo rasterizer;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		rasterizer.frontFace = vk::FrontFace::eClockwise;
		rasterizer.depthBiasEnable = VK_FALSE;

		vk::PipelineMultisampleStateCreateInfo multisampling;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

		// Enable alpha blending for UI transparency
		vk::PipelineColorBlendAttachmentState colorBlendAttachment;
		colorBlendAttachment.colorWriteMask =
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = true;
		colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
		colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
		colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
		colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
		colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
		colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

		vk::PipelineColorBlendStateCreateInfo color_blending;
		color_blending.logicOpEnable = false;
		color_blending.logicOp = vk::LogicOp::eCopy;
		color_blending.attachmentCount = 1;
		color_blending.pAttachments = &colorBlendAttachment;
		color_blending.blendConstants[0] = 0.0f;
		color_blending.blendConstants[1] = 0.0f;
		color_blending.blendConstants[2] = 0.0f;
		color_blending.blendConstants[3] = 0.0f;

		// Push constant range for projection matrix
		vk::PushConstantRange push_constant_range;
		push_constant_range.stageFlags = vk::ShaderStageFlagBits::eVertex;
		push_constant_range.offset = 0;
		push_constant_range.size = sizeof(PushConstants);

		// Pipeline layout with optional descriptor set and push constants
		vk::PipelineLayoutCreateInfo pipeline_layout_info;
		pipeline_layout_info.setLayoutCount = desc_layout ? 1 : 0;
		pipeline_layout_info.pSetLayouts = desc_layout ? &desc_layout->handle : nullptr;
		pipeline_layout_info.pushConstantRangeCount = 1;
		pipeline_layout_info.pPushConstantRanges = &push_constant_range;

		layout = p_device->device.createPipelineLayout(pipeline_layout_info);

		vec<vk::DynamicState> dynamic_states = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };

		vk::PipelineDynamicStateCreateInfo dynamic_info;
		dynamic_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
		dynamic_info.pDynamicStates = dynamic_states.data();

		vk::GraphicsPipelineCreateInfo pipeline_info;
		pipeline_info.stageCount = 2;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &input_assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multisampling;
		pipeline_info.pColorBlendState = &color_blending;
		pipeline_info.pDynamicState = &dynamic_info;
		pipeline_info.layout = layout;
		pipeline_info.renderPass = p_render_pass->handle;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

		handle = p_device->device.createGraphicsPipeline(VK_NULL_HANDLE, pipeline_info).value;
	}

	Pipeline::~Pipeline()
	{
		if (p_device == nullptr) return;
		if (handle)
			p_device->device.destroyPipeline(handle);
		if (layout)
			p_device->device.destroyPipelineLayout(layout);
	}
}
