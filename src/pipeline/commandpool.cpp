#include "Teapot/pipeline/commandpool.hpp"
#include "Teapot/core/device.hpp"
#include "Teapot/core/dispatch.hpp"
#include "Teapot/core/swapchain.hpp"
#include "Teapot/pipeline/renderpass.hpp"
#include "Teapot/pipeline/framebuffer.hpp"
#include "Teapot/ui/ui_renderer.hpp"
#include "Teapot/platform/window.hpp"
#include "Teapot/common/structures.hpp"

namespace Teapot
{
	CommandPool::CommandPool(Init& init, vkb::QueueType queue_type)
		: p_device(init.p_device)
	{
		vk::CommandPoolCreateInfo pool_info = {};

		auto queue_index_ret = init.p_device->handle.get_queue_index(queue_type);
		if (!queue_index_ret) {
			err("Failed to get queue index for command pool");
		}
		pool_info.queueFamilyIndex = queue_index_ret.value();
		pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		VkCommandPoolCreateInfo raw_pool_info = pool_info;
		VkCommandPool raw_pool;
		isVkOk(p_device->table->handle.createCommandPool(&raw_pool_info, nullptr, &raw_pool), "Failed to create command pool");
		handle = raw_pool;
	}

	CommandPool::~CommandPool()
	{
		if (handle != VK_NULL_HANDLE && p_device != nullptr) {
			p_device->table->handle.destroyCommandPool(handle, nullptr);
		}
	}
	
	void CommandPool::allocateCommandBuffers(uint32_t count)
	{
		buffers.resize(count);

		vk::CommandBufferAllocateInfo alloc_info = {};
		alloc_info.commandPool = handle;
		alloc_info.level = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandBufferCount = count;
		
		VkCommandBufferAllocateInfo raw_alloc_info = alloc_info;
		vec<VkCommandBuffer> raw_buffers(count);
		isVkOk(
			p_device->table->handle.allocateCommandBuffers(&raw_alloc_info, raw_buffers.data()),
			"Failed to allocate command buffers"
		);
		for (size_t i = 0; i < count; i++) {
			buffers[i] = raw_buffers[i];
		}
	}

	void CommandPool::recordUICommands(
		uint32_t image_index,
		RenderPass& render_pass,
		Framebuffer* framebuffers,
		UIRenderer& ui_renderer,
		Window& window
	)
	{
		vk::CommandBuffer cmd = buffers[image_index];

		p_device->table->handle.resetCommandBuffer(cmd, 0);

		vk::CommandBufferBeginInfo begin_info = {};
		VkCommandBufferBeginInfo raw_begin_info = begin_info;
		isVkOk(
			p_device->table->handle.beginCommandBuffer(cmd, &raw_begin_info),
			"Failed to begin command buffer"
		);

		vk::RenderPassBeginInfo render_pass_info = {};
		render_pass_info.renderPass = render_pass.handle;
		render_pass_info.framebuffer = framebuffers[image_index].handle;
		vk::Offset2D offset{ 0, 0 };
		render_pass_info.renderArea.offset = offset;
		render_pass_info.renderArea.extent = p_device->swapchain->handle.extent;

		vk::ClearColorValue color = { 0.0f, 0.0f, 0.0f, 1.0f };
		vk::ClearValue clear_color = color;
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_color;

		VkRenderPassBeginInfo raw_render_pass_info = render_pass_info;
		p_device->table->handle.cmdBeginRenderPass(cmd, &raw_render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		// Set dynamic viewport and scissor
		vk::Viewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(p_device->swapchain->handle.extent.width);
		viewport.height = static_cast<float>(p_device->swapchain->handle.extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkViewport raw_viewport = viewport;
		p_device->table->handle.cmdSetViewport(cmd, 0, 1, &raw_viewport);

		vk::Rect2D scissor = {};
		vk::Offset2D sc_offset{ 0, 0 };
		scissor.offset = sc_offset;
		scissor.extent = p_device->swapchain->handle.extent;
		VkRect2D raw_scissor = scissor;
		p_device->table->handle.cmdSetScissor(cmd, 0, 1, &raw_scissor);

		// Record UI rendering commands
		ui_renderer.recordCommands(cmd, window.getSize(), window.getHeight());

		// End render pass and command buffer
		p_device->table->handle.cmdEndRenderPass(cmd);

		isVkOk(
			p_device->table->handle.endCommandBuffer(cmd),
			"Failed to end command buffer"
		);
	}
}
