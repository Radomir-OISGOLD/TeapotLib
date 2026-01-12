#include "Teapot/pipeline/framebuffer.hpp"
#include "Teapot/core/device.hpp"
#include "Teapot/core/image.hpp"
#include "Teapot/pipeline/renderpass.hpp"
#include "Teapot/core/swapchain.hpp"
#include "Teapot/core/dispatch.hpp"
#include "Teapot/common/structures.hpp"

namespace Teapot
{
	Framebuffer::Framebuffer(RenderData* render_data, Image& image)
		: p_device(render_data->p_device)
	{
		VkImageView attachments[] = { image.view };

		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = render_data->p_render_pass->handle;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = attachments;
		framebuffer_info.width = render_data->p_swapchain->handle.extent.width;
		framebuffer_info.height = render_data->p_swapchain->handle.extent.height;
		framebuffer_info.layers = 1;

		isVkOk(p_device->table->handle.createFramebuffer(&framebuffer_info, nullptr, &handle), "Failed to create framebuffer");
	}

	Framebuffer::~Framebuffer()
	{
		if (handle != VK_NULL_HANDLE && p_device != nullptr) {
			p_device->table->handle.destroyFramebuffer(handle, nullptr);
		}
	}
}
