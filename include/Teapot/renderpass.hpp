#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	struct RenderPass
	{
		RenderPass(Device* p_device, Swapchain* p_swapchain);
		~RenderPass();

		// Movable but not copyable
		RenderPass(const RenderPass&) = delete;
		RenderPass& operator=(const RenderPass&) = delete;

		vk::RenderPass handle;
		Device* p_device = nullptr; // non-owning
	};
}
