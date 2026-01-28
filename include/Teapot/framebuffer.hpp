#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	struct Framebuffer
	{
		Framebuffer(Device* p_device, RenderPass* p_render_pass, Swapchain* p_swapchain, Image& image);
		~Framebuffer();

		// Movable but not copyable
		Framebuffer(const Framebuffer&) = delete;
		Framebuffer& operator=(const Framebuffer&) = delete;
		Framebuffer(Framebuffer&&) noexcept = default;
		Framebuffer& operator=(Framebuffer&&) noexcept = default;

		vk::Framebuffer handle;
		Device* p_device = nullptr; // non-owning
	};
}
