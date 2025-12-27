
#include "Teapot/rendering.hpp"
#include "Teapot/device.hpp"
#include "VkBootstrap.h"
#include "vulkan/vulkan.hpp"
#include <fstream>

namespace Teapot
{
	Queue::Queue(Device device, vkb::QueueType type)
	{
		auto queue_ret = device.handle.get_queue(type);
		if (!queue_ret) {
			std::cerr << "Failed to get queue. Error: " << queue_ret.error().message() << "\n";
			return;
		}
		handle = queue_ret.value();
	}

	Queue::~Queue()
	{
		delete this;
	}

	Swapchain::Swapchain(Device device)
	{
		vkb::SwapchainBuilder swapchain_builder{ device.handle };
		auto swap_ret = swapchain_builder.build();
		if (!swap_ret) {
			std::cout << swap_ret.error().message() << "\n";
			delete this;
			return;
		}
		handle = swap_ret.value();
	}

	Swapchain::~Swapchain()
	{
		vkb::destroy_swapchain(handle);
	}

	DispatchTable::DispatchTable(Device& device)
	{
		handle = device.handle.make_table();
		p_device = &device;
	}

	namespace
	{
		std::vector<char> readFile(const std::string& filename) {
			std::ifstream file(filename, std::ios::ate | std::ios::binary);
		
			if (!file.is_open()) {
				throw std::runtime_error("failed to open file!");
			}
		
			size_t file_size = (size_t)file.tellg();
			std::vector<char> buffer(file_size);
		
			file.seekg(0);
			file.read(buffer.data(), static_cast<std::streamsize>(file_size));
		
			file.close();
		
			return buffer;
		}
	}

	Shader::Shader(DispatchTable& table, const char* file_path) :
		p_table(&table)
	{
		code = readFile(file_path);

		VkShaderModuleCreateInfo create_info = {};
   		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    	create_info.codeSize = code.size();
    	create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    	VkShaderModule shader_module;
    	if (table.handle.createShaderModule(&create_info, nullptr, &shader_module) != VK_SUCCESS) {
        	err("Failed to create shader.");
    	}
		vk_handle = shader_module;

	}
	Shader::Shader(DispatchTable& table, std::vector<char> inline_shader) :
		p_table(&table)
	{
		VkShaderModuleCreateInfo create_info = {};
   		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    	create_info.codeSize = code.size();
    	create_info.pCode = reinterpret_cast<const uint32_t*>(inline_shader.data());

    	VkShaderModule shader_module;
    	if (table.handle.createShaderModule(&create_info, nullptr, &shader_module) != VK_SUCCESS) {
        	err("Failed to create shader.");
    	}
		vk_handle = shader_module;

	}

	Shader::~Shader()
	{
		p_table->handle.destroyShaderModule(vk_handle, nullptr);
	}


	CommandPool::CommandPool(DispatchTable& table, vkb::QueueType queue_type) :
		p_table(&table)
	{
		VkCommandPool pool;
		VkCommandPoolCreateInfo pool_info = {};
    	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    	pool_info.queueFamilyIndex = table.p_device->handle.get_queue_index(vkb::QueueType::graphics).value();

    	if (table.handle.createCommandPool(&pool_info, nullptr, &pool) != VK_SUCCESS) {
    	    std::cout << "failed to create command pool\n";
			delete this;
			return;
    	}

		vk_handle = pool;
	}


	CommandPool::~CommandPool()
	{
		p_table->handle.destroyCommandPool(vk_handle, nullptr);
	}

	// Very simple approach - expects just 2 main shaders (frag and vert)
	Pipeline::Pipeline(Swapchain& swapchain, Shader& sh_vert, Shader& shader_frag)
	{
		VkPipelineShaderStageCreateInfo vert_stage_info = {};
		vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vert_stage_info.module = sh_vert.vk_handle;
		vert_stage_info.pName = "main";
	
		VkPipelineShaderStageCreateInfo frag_stage_info = {};
		frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		frag_stage_info.module = shader_frag.vk_handle;
		frag_stage_info.pName = "main";
	
		VkPipelineShaderStageCreateInfo shader_stages[] = { vert_stage_info, frag_stage_info };

		
		VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
    	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    	vertex_input_info.vertexBindingDescriptionCount = 0;
    	vertex_input_info.vertexAttributeDescriptionCount = 0;

    	VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
    	input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    	input_assembly.primitiveRestartEnable = VK_FALSE;

    	viewport.x = 0.0f;
    	viewport.y = 0.0f;
    	viewport.width = (float)swapchain.handle.extent.width;
    	viewport.height = (float)swapchain.handle.extent.height;
    	viewport.minDepth = 0.0f;
    	viewport.maxDepth = 1.0f;

		scissor.offset = { 0, 0 };
    	scissor.extent = swapchain.handle.extent;

		VkPipelineViewportStateCreateInfo viewport_state = {};
    	viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    	viewport_state.viewportCount = 1;
    	viewport_state.pViewports = &viewport;
    	viewport_state.scissorCount = 1;
    	viewport_state.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
    	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    	rasterizer.depthClampEnable = VK_FALSE;
    	rasterizer.rasterizerDiscardEnable = VK_FALSE;
    	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    	rasterizer.lineWidth = 1.0f;
    	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    	rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
    	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    	multisampling.sampleShadingEnable = VK_FALSE;
    	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo color_blending = {};
		color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blending.logicOpEnable = VK_FALSE;
		color_blending.logicOp = VK_LOGIC_OP_COPY;
		color_blending.attachmentCount = 1;
		color_blending.pAttachments = &colorBlendAttachment;
		color_blending.blendConstants[0] = 0.0f;
		color_blending.blendConstants[1] = 0.0f;
		color_blending.blendConstants[2] = 0.0f;
		color_blending.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipeline_layout_info = {};
		pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_info.setLayoutCount = 0;
		pipeline_layout_info.pushConstantRangeCount = 0;

		if (sh_vert.p_table->handle.createPipelineLayout(&pipeline_layout_info, nullptr, p_layout) != VK_SUCCESS) {
			err("Failed to create pipeline layout");
		}

		vec<VkDynamicState> dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo dynamic_info = {};
		dynamic_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
		dynamic_info.pDynamicStates = dynamic_states.data();
	
		VkGraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = 2;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &input_assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multisampling;
		pipeline_info.pColorBlendState = &color_blending;
		pipeline_info.pDynamicState = &dynamic_info;
		pipeline_info.layout = *p_layout;
		pipeline_info.renderPass = *p_render_pass;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
	
		if (sh_vert.p_table->handle.createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipeline_info, nullptr, vk_handle) != VK_SUCCESS) {
			err("Failed to create pipeline");
		}

	}

	int drawFrame()
	{
		
	}

}
