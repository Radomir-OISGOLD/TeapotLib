#include "Teapot/framebuffer.hpp"
#include "Teapot/device.hpp"
#include "Teapot/image.hpp"
#include "Teapot/renderpass.hpp"
#include "Teapot/swapchain.hpp"

namespace Teapot
{
	Framebuffer::Framebuffer(Device* p_device, RenderPass* p_render_pass, Swapchain* p_swapchain, Image& image)
		: p_device(p_device)
	{
		vk::ImageView attachments[] = { image.view };

		vk::FramebufferCreateInfo framebuffer_info;
		framebuffer_info.renderPass = p_render_pass->handle;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = attachments;
		framebuffer_info.width = p_swapchain->vkb_swapchain.extent.width;
		framebuffer_info.height = p_swapchain->vkb_swapchain.extent.height;
		framebuffer_info.layers = 1;

		handle = p_device->device.createFramebuffer(framebuffer_info);
	}

	Framebuffer::~Framebuffer()
	{
		if (handle && p_device)
			p_device->device.destroyFramebuffer(handle);
	}
}
