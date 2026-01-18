#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	struct CommandPool
	{
		CommandPool(Init& init, vkb::QueueType queue_type);
		~CommandPool();

		CommandPool(const CommandPool&) = delete;
		CommandPool& operator=(const CommandPool&) = delete;
		CommandPool(CommandPool&&) = default;
		CommandPool& operator=(CommandPool&&) = default;

		//void allocBuffers(Pipeline& pipeline);

		void allocateCommandBuffers(uint32_t count);
		void recordUICommands(
			uint32_t image_index,
			RenderPass& render_pass,
			Framebuffer* framebuffers,
			UIRenderer& ui_renderer,
			Window& window
		);

		vk::CommandPool handle = VK_NULL_HANDLE;
		vec<vk::CommandBuffer> buffers;

		Device* p_device = nullptr;
	};
}
