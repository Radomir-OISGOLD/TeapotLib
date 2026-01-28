
#pragma once

#include "Teapot/cap.hpp"
#include "Teapot/vertex.hpp"

namespace Teapot
{
	struct UIQuad
	{
		vec<UIVertex> vertices;
		vec<uint16_t> indices;
		vk::DescriptorSet descriptor_set;
	};

	class UIRenderer
	{
	public:
		UIRenderer(Device* p_device, RenderPass* p_render_pass, Swapchain* p_swapchain);
		~UIRenderer();

		UIRenderer(const UIRenderer&) = delete;
		UIRenderer& operator=(const UIRenderer&) = delete;

		void createPipeline(Shader& vert, Shader& frag, DescriptorSetLayout& desc_layout);
		void createBuffers();

		// Build draw list from buttons
		void prepareFrame(
			const vec<std::unique_ptr<Button>>& buttons,
			const Window& window,
			DescriptorPool& desc_pool,
			DescriptorSetLayout& desc_layout
		);

		// Record draw commands into command buffer
		void recordCommands(vk::CommandBuffer cmd_buffer, uint32_t window_width, uint32_t window_height);

	private:
		Device* p_device = nullptr;
		RenderPass* p_render_pass = nullptr;
		Swapchain* p_swapchain = nullptr;

		std::unique_ptr<Pipeline> pipeline;

		vk::Buffer vertex_buffer;
		VmaAllocation vertex_allocation = VK_NULL_HANDLE;
		vk::Buffer index_buffer;
		VmaAllocation index_allocation = VK_NULL_HANDLE;

		vec<UIQuad> draw_list;

		void createQuadGeometry();
		vk::Buffer createBuffer(
			vk::DeviceSize size,
			vk::BufferUsageFlags usage,
			VmaMemoryUsage memory_usage,
			VmaAllocationCreateFlags flags,
			VmaAllocation& allocation
		);
	};
}

