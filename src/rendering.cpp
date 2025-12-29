
#include "Teapot/rendering.hpp"
#include "Teapot/cap.hpp"
#include "Teapot/device.hpp"
#include "Teapot/image.hpp"
#include <cstddef>
#include <fstream>

namespace Teapot
{
	Queue::Queue(Device& device, vkb::QueueType type)
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

	Swapchain::Swapchain(Device& device)
	{
		vkb::SwapchainBuilder swapchain_builder{ device.handle };
		auto swap_ret = swapchain_builder.build();
		if (!swap_ret) {
			std::cout << swap_ret.error().message() << "\n";
			delete this;
			return;
		}

		handle = swap_ret.value();

		auto raw_images = handle.get_images().value();
		auto raw_views = handle.get_image_views().value();

		for (size_t i = 0; i < raw_images.size(); i++)
		{
			Image img = Image(raw_images[i], raw_views[i]);
			swapchain_images.push_back(&img);
		}
		
		device.p_swapchain = this;
	}

	Swapchain::~Swapchain()
	{
		vkb::destroy_swapchain(handle);
	}

	void Swapchain::initSemaphores(DispatchTable& table)
	{
		available_semaphores.resize(TEAPOT_DOUBLE_BUFFERING);
		finished_semaphore.resize(handle.image_count);
		in_flight_fences.resize(TEAPOT_DOUBLE_BUFFERING);
		image_in_flight.resize(handle.image_count, VK_NULL_HANDLE);
	
		VkSemaphoreCreateInfo semaphore_info = {};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
		VkFenceCreateInfo fence_info = {};
		fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	
		for (size_t i = 0; i < handle.image_count; i++) {
			if (table.handle.createSemaphore(&semaphore_info, nullptr, &finished_semaphore[i]) != VK_SUCCESS) {
				err("Failed to init semaphores.");
			}
		}
	
		for (size_t i = 0; i < TEAPOT_DOUBLE_BUFFERING; i++) {
			if (table.handle.createSemaphore(&semaphore_info, nullptr, &finished_semaphore[i]) != VK_SUCCESS ||
				table.handle.createFence(&fence_info, nullptr, &in_flight_fences[i]) != VK_SUCCESS) {
				err("Failed to init semaphores.");
			}
		}
	}


	DispatchTable::DispatchTable(Device& device)
	{
		handle = device.handle.make_table();
		p_device = &device;
	}

	DispatchTable::~DispatchTable()
	{
		// for (Framebuffer* buf : framebuffers)
		// {
		// 	//destroyFramebuffer(buf->handle, nullptr);
		// }
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
		handle = shader_module;

	}
	Shader::Shader(DispatchTable& table, std::string inline_shader) :
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
		handle = shader_module;

	}

	Shader::~Shader()
	{
		p_table->handle.destroyShaderModule(handle, nullptr);
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

		handle = pool;
	}


	CommandPool::~CommandPool()
	{
		p_table->handle.destroyCommandPool(handle, nullptr);
	}

	void CommandPool::allocBuffers(Pipeline& pipeline)
	{
		VkExtent2D extent = p_table->p_device->p_swapchain->handle.extent;
		buffers.resize(p_table->framebuffers.size());
		
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = handle;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)p_table->framebuffers.size();
	
		if (p_table->handle.allocateCommandBuffers(&allocInfo, buffers.data()) != VK_SUCCESS)
		{
			err("Failed to allocate command buffers.");
		}
	
		for (size_t i = 0; i < buffers.size(); i++) {
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
			if (p_table->handle.beginCommandBuffer(buffers[i], &begin_info) != VK_SUCCESS)
			{
				err("Failed to begin recording to a cmd buffer.");
			}
	
			VkRenderPassBeginInfo render_pass_info = {};
			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_info.renderPass = p_table->p_rend_pass->handle;
			render_pass_info.framebuffer = p_table->framebuffers[i]->handle;
			render_pass_info.renderArea.offset = { 0, 0 };
			render_pass_info.renderArea.extent = extent;
			VkClearValue clearColor{ { { 0.0f, 0.0f, 0.0f, 1.0f } } };
			render_pass_info.clearValueCount = 1;
			render_pass_info.pClearValues = &clearColor;
	
			VkViewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = extent.width;
			viewport.height = extent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkRect2D scissor = {};
			scissor.offset = { 0, 0 };
			scissor.extent = extent;
	
			p_table->handle.cmdSetViewport(buffers[i], 0, 1, &viewport);
			p_table->handle.cmdSetScissor(buffers[i], 0, 1, &scissor);
	
			p_table->handle.cmdBeginRenderPass(buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
	
			p_table->handle.cmdBindPipeline(buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.handle);
	
			p_table->handle.cmdDraw(buffers[i], 3, 1, 0, 0);
	
			p_table->handle.cmdEndRenderPass(buffers[i]);
	
			if (p_table->handle.endCommandBuffer(buffers[i]) != VK_SUCCESS)
			{
				err("Failed to record command buffer.");
			}
		}	
	}


	// Very simple approach - expects just 2 main shaders (frag and vert)
	Pipeline::Pipeline(Swapchain& swapchain, Shader& sh_vert, Shader& shader_frag)
	{
		VkPipelineShaderStageCreateInfo vert_stage_info = {};
		vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vert_stage_info.module = sh_vert.handle;
		vert_stage_info.pName = "main";
	
		VkPipelineShaderStageCreateInfo frag_stage_info = {};
		frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		frag_stage_info.module = shader_frag.handle;
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
	
		if (sh_vert.p_table->handle.createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &handle) != VK_SUCCESS) {
			err("Failed to create pipeline");
		}
		p_device = swapchain.p_device;
	}

	Framebuffer::Framebuffer(DispatchTable& table, Swapchain& swapchain, RenderPass& render_pass) :
		p_table(&table)
	{
		//handles.resize(swapchain.swapchain_images.size());
	
		for (size_t i = 0; i < swapchain.swapchain_images.size(); i++)
		{
			VkImageView attachments[] = { swapchain.swapchain_images[i]->view };
	
			VkFramebufferCreateInfo framebuffer_info = {};
			framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_info.renderPass = render_pass.handle;
			framebuffer_info.attachmentCount = 1;
			framebuffer_info.pAttachments = attachments;
			framebuffer_info.width = swapchain.handle.extent.width;
			framebuffer_info.height = swapchain.handle.extent.height;
			framebuffer_info.layers = 1;
	
			if (table.handle.createFramebuffer(&framebuffer_info, nullptr, &handle) != VK_SUCCESS)
			{
				err("Failed to create framebuffer");
			}
			table.framebuffers.push_back(this);
		}
	}

	Framebuffer::~Framebuffer()
	{

	}

	RenderPass::RenderPass(Swapchain& swapchain, DispatchTable& table)
	{
		VkAttachmentDescription color_attachment = {};
		color_attachment.format = swapchain.handle.image_format;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	
		VkAttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;
	
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	
		VkRenderPassCreateInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = 1;
		render_pass_info.pAttachments = &color_attachment;
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;
		render_pass_info.dependencyCount = 1;
		render_pass_info.pDependencies = &dependency;
	
		if (table.handle.createRenderPass(&render_pass_info, nullptr, &handle) != VK_SUCCESS) {
			err("Failed to create render pass.");
		}
	}

	int drawFrame(size_t* p_frame, DispatchTable& table, Swapchain& chain, )
	{
		table.handle.waitForFences(1, &chain.in_flight_fences[*p_frame], VK_TRUE, UINT64_MAX);

		uint32_t image_index = 0;
		VkResult result = init.disp.acquireNextImageKHR(
			init.swapchain, UINT64_MAX, data.available_semaphores[data.current_frame], VK_NULL_HANDLE, &image_index);
	
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			return recreate_swapchain(init, data);
		} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			std::cout << "failed to acquire swapchain image. Error " << result << "\n";
			return -1;
		}
	
		if (data.image_in_flight[image_index] != VK_NULL_HANDLE) {
			init.disp.waitForFences(1, &data.image_in_flight[image_index], VK_TRUE, UINT64_MAX);
		}
		data.image_in_flight[image_index] = data.in_flight_fences[data.current_frame];
	
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
		VkSemaphore wait_semaphores[] = { data.available_semaphores[data.current_frame] };
		VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = wait_semaphores;
		submitInfo.pWaitDstStageMask = wait_stages;
	
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &data.command_buffers[image_index];
	
		VkSemaphore signal_semaphores[] = { data.finished_semaphore[image_index] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signal_semaphores;
	
		init.disp.resetFences(1, &data.in_flight_fences[data.current_frame]);
	
		if (init.disp.queueSubmit(data.graphics_queue, 1, &submitInfo, data.in_flight_fences[data.current_frame]) != VK_SUCCESS) {
			std::cout << "failed to submit draw command buffer\n";
			return -1; //"failed to submit draw command buffer
		}
	
		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = signal_semaphores;
	
		VkSwapchainKHR swapChains[] = { init.swapchain };
		present_info.swapchainCount = 1;
		present_info.pSwapchains = swapChains;
	
		present_info.pImageIndices = &image_index;
	
		result = init.disp.queuePresentKHR(data.present_queue, &present_info);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			return recreate_swapchain(init, data);
		} else if (result != VK_SUCCESS) {
			std::cout << "failed to present swapchain image\n";
			return -1;
		}
	
		data.current_frame = (data.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
		return 0;
	}

}
