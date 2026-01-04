#include "Teapot/pipeline/framebuffer.hpp"
#include "Teapot/core/device.hpp"
#include "Teapot/core/image.hpp"
#include "Teapot/pipeline/renderpass.hpp"
#include "Teapot/core/swapchain.hpp"

namespace Teapot
{
	Framebuffer::Framebuffer(Device& device, RenderPass& render_pass, Image& image)
		: p_device(&device)
	{
		VkImageView attachments[] = { image.view };

		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = render_pass.handle;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = attachments;
		framebuffer_info.width = device.swapchain->handle.extent.width;
		framebuffer_info.height = device.swapchain->handle.extent.height;
		framebuffer_info.layers = 1;

		isVkOk(p_device->table->createFramebuffer(&framebuffer_info, nullptr, &handle), "Failed to create framebuffer");
	}

	Framebuffer::~Framebuffer()
	{
		if (handle != VK_NULL_HANDLE && p_device != nullptr) {
			p_device->table->destroyFramebuffer(handle, nullptr);
		}
	}
}
