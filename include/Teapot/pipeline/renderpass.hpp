#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	struct RenderPass
	{
		RenderPass(Init* init);
		~RenderPass();

		// Movable but not copyable
		RenderPass(const RenderPass&) = delete;
		RenderPass& operator=(const RenderPass&) = delete;
		RenderPass(RenderPass&&) = default;
		RenderPass& operator=(RenderPass&&) = default;

		VkRenderPass handle = VK_NULL_HANDLE;
		Device* p_device = nullptr; // non-owning
	};
}
