#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	struct CommandPool
	{
		CommandPool(Device& device, vkb::QueueType queue_type);
		~CommandPool();

		// Movable but not copyable
		CommandPool(const CommandPool&) = delete;
		CommandPool& operator=(const CommandPool&) = delete;
		CommandPool(CommandPool&&) = default;
		CommandPool& operator=(CommandPool&&) = default;

		void allocBuffers(Pipeline& pipeline); // This will need to be refactored

		// New methods for UI rendering
		void allocateCommandBuffers(uint32_t count);
		void recordUICommands(
			uint32_t image_index,
			RenderPass& render_pass,
			Framebuffer* framebuffers,
			UIRenderer& ui_renderer,
			Window& window
		);

		VkCommandPool handle = VK_NULL_HANDLE;
		vec<VkCommandBuffer> buffers;

		Device* p_device = nullptr; // non-owning
	};
}
