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
	CommandPool::CommandPool(Init* init, vkb::QueueType queue_type)
		: p_device(init->p_device)
	{
		VkCommandPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

		auto queue_index_ret = init->p_device->handle.get_queue_index(queue_type);
		if (!queue_index_ret) {
			err("Failed to get queue index for command pool");
		}
		pool_info.queueFamilyIndex = queue_index_ret.value();
		pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Allow resetting individual command buffers

		isVkOk(p_device->table->handle.createCommandPool(&pool_info, nullptr, &handle), "Failed to create command pool");
	}

	CommandPool::~CommandPool()
	{
		if (handle != VK_NULL_HANDLE && p_device != nullptr) {
			// Command buffers are implicitly freed with the pool
			p_device->table->handle.destroyCommandPool(handle, nullptr);
		}
	}
	
	// The allocBuffers function needs a major redesign to fit the new structure.
	void CommandPool::allocBuffers(Pipeline& pipeline)
	{
		// TODO: Re-implement this.
		// This function was tightly coupled with the old monolithic structure.
		// A proper implementation requires a redesign of the render loop itself.
	}

	void CommandPool::allocateCommandBuffers(uint32_t count)
	{
		buffers.resize(count);

		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.commandPool = handle;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandBufferCount = count;

		isVkOk(
			p_device->table->handle.allocateCommandBuffers(&alloc_info, buffers.data()),
			"Failed to allocate command buffers"
		);
	}

	void CommandPool::recordUICommands(
		uint32_t image_index,
		RenderPass& render_pass,
		Framebuffer* framebuffers,
		UIRenderer& ui_renderer,
		Window& window
	)
	{
		VkCommandBuffer cmd = buffers[image_index];

		// Reset command buffer
		p_device->table->handle.resetCommandBuffer(cmd, 0);

		// Begin recording
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = 0;

		isVkOk(
			p_device->table->handle.beginCommandBuffer(cmd, &begin_info),
			"Failed to begin command buffer"
		);

		// Begin render pass
		VkRenderPassBeginInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = render_pass.handle;
		render_pass_info.framebuffer = framebuffers[image_index].handle;
		render_pass_info.renderArea.offset = {0, 0};
		render_pass_info.renderArea.extent = p_device->swapchain->handle.extent;

		VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_color;

		p_device->table->handle.cmdBeginRenderPass(cmd, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		// Set dynamic viewport and scissor
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(p_device->swapchain->handle.extent.width);
		viewport.height = static_cast<float>(p_device->swapchain->handle.extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		p_device->table->handle.cmdSetViewport(cmd, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = {0, 0};
		scissor.extent = p_device->swapchain->handle.extent;
		p_device->table->handle.cmdSetScissor(cmd, 0, 1, &scissor);

		// Record UI rendering commands
		ui_renderer.recordCommands(cmd, window.getWidth(), window.getHeight());

		// End render pass and command buffer
		p_device->table->handle.cmdEndRenderPass(cmd);

		isVkOk(
			p_device->table->handle.endCommandBuffer(cmd),
			"Failed to end command buffer"
		);
	}
}
