#include "Teapot/renderpass.hpp"
#include "Teapot/device.hpp"
#include "Teapot/swapchain.hpp"

namespace Teapot
{
	RenderPass::RenderPass(Device* p_device, Swapchain* p_swapchain)
		: p_device(p_device)
	{
		vk::AttachmentDescription color_attachment;
		color_attachment.format = static_cast<vk::Format>(p_swapchain->vkb_swapchain.image_format);
		color_attachment.samples = vk::SampleCountFlagBits::e1;
		color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
		color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
		color_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		color_attachment.initialLayout = vk::ImageLayout::eUndefined;
		color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
	
		vk::AttachmentReference color_attachment_ref;
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = vk::ImageLayout::eColorAttachmentOptimal;
	
		vk::SubpassDescription subpass;
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;
	
		vk::SubpassDependency dependency;
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.srcAccessMask = {};
		dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
	
		vk::RenderPassCreateInfo render_pass_info;
		render_pass_info.attachmentCount = 1;
		render_pass_info.pAttachments = &color_attachment;
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;
		render_pass_info.dependencyCount = 1;
		render_pass_info.pDependencies = &dependency;
	
		handle = p_device->device.createRenderPass(render_pass_info);
	}

	RenderPass::~RenderPass()
	{
		if (handle && p_device)
			p_device->device.destroyRenderPass(handle);
	}
}
