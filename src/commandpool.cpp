#include "Teapot/commandpool.hpp"
#include "Teapot/device.hpp"
#include "Teapot/swapchain.hpp"
#include "Teapot/renderpass.hpp"
#include "Teapot/framebuffer.hpp"
#include "Teapot/ui_renderer.hpp"
#include "Teapot/window.hpp"

namespace Teapot
{
	CommandPool::CommandPool(Device* p_device, vkb::QueueType queue_type)
		: p_device(p_device)
	{
		vk::CommandPoolCreateInfo pool_info;

		auto queue_index_ret = p_device->vkb_device.get_queue_index(queue_type);
		if (!queue_index_ret)
			err("Failed to get queue index for command pool");
		
		pool_info.queueFamilyIndex = queue_index_ret.value();
		pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		
		handle = p_device->device.createCommandPool(pool_info);
	}

	CommandPool::~CommandPool()
	{
		if (handle && p_device)
			p_device->device.destroyCommandPool(handle);
	}
	
	void CommandPool::allocateCommandBuffers(uint32_t count)
	{
		buffers.resize(count);

		vk::CommandBufferAllocateInfo alloc_info;
		alloc_info.commandPool = handle;
		alloc_info.level = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandBufferCount = count;
		
		buffers = p_device->device.allocateCommandBuffers(alloc_info);
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

		cmd.reset();

		vk::CommandBufferBeginInfo begin_info;
		cmd.begin(begin_info);

		vk::RenderPassBeginInfo render_pass_info;
		render_pass_info.renderPass = render_pass.handle;
		render_pass_info.framebuffer = framebuffers[image_index].handle;
		vk::Offset2D offset{ 0, 0 };
		render_pass_info.renderArea.offset = offset;
		render_pass_info.renderArea.extent = p_device->swapchain->vkb_swapchain.extent;

		vk::ClearColorValue color = { 0.0f, 0.0f, 0.0f, 1.0f };
		vk::ClearValue clear_color = color;
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_color;

		cmd.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);

		// Set dynamic viewport and scissor
		vk::Viewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(p_device->swapchain->vkb_swapchain.extent.width);
		viewport.height = static_cast<float>(p_device->swapchain->vkb_swapchain.extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		cmd.setViewport(0, { viewport });

		vk::Rect2D scissor;
		vk::Offset2D sc_offset{ 0, 0 };
		scissor.offset = sc_offset;
		scissor.extent = p_device->swapchain->vkb_swapchain.extent;
		cmd.setScissor(0, { scissor });

		// Record UI rendering commands
		ui_renderer.recordCommands(cmd, window.size.x, window.size.y);

		// End render pass and command buffer
		cmd.endRenderPass();

		cmd.end();
	}
}
