#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	struct Framebuffer
	{
		Framebuffer(RenderData* render_data, Image& image);
		~Framebuffer();

		// Movable but not copyable
		Framebuffer(const Framebuffer&) = delete;
		Framebuffer& operator=(const Framebuffer&) = delete;
		Framebuffer(Framebuffer&&) = default;
		Framebuffer& operator=(Framebuffer&&) = default;

		VkFramebuffer handle = VK_NULL_HANDLE;
		Device* p_device = nullptr; // non-owning
	};
}
