#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	struct CommandPool
	{
		CommandPool(Device* p_device, vkb::QueueType queue_type);
		~CommandPool();

		CommandPool(const CommandPool&) = delete;
		CommandPool& operator=(const CommandPool&) = delete;

		void allocateCommandBuffers(uint32_t count);
		void recordUICommands(
			uint32_t image_index,
			RenderPass& render_pass,
			Framebuffer* framebuffers,
			UIRenderer& ui_renderer,
			Window& window
		);

		vk::CommandPool handle;
		vec<vk::CommandBuffer> buffers;

		Device* p_device = nullptr;
	};
}
