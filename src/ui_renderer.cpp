#include "Teapot/ui_renderer.hpp"
#include "Teapot/button.hpp"
#include "Teapot/projection.hpp"
#include "Teapot/device.hpp"
#include "Teapot/swapchain.hpp"
#include "Teapot/image.hpp"
#include "Teapot/renderpass.hpp"
#include "Teapot/pipeline.hpp"
#include "Teapot/descriptors.hpp"
#include "Teapot/window.hpp"

namespace Teapot
{
	UIRenderer::UIRenderer(Device* p_device, RenderPass* p_render_pass, Swapchain* p_swapchain)
		: p_device(p_device)
		, p_render_pass(p_render_pass)
		, p_swapchain(p_swapchain)
	{
	}

	UIRenderer::~UIRenderer()
	{
		if (p_device == nullptr) return;

		if (vertex_buffer && vertex_allocation)
			vmaDestroyBuffer(p_device->allocator, static_cast<VkBuffer>(vertex_buffer), vertex_allocation);
		if (index_buffer && index_allocation)
			vmaDestroyBuffer(p_device->allocator, static_cast<VkBuffer>(index_buffer), index_allocation);
	}

	void UIRenderer::createPipeline(Shader& vert, Shader& frag, DescriptorSetLayout& desc_layout)
	{
		// Create pipeline with vertex input and descriptor set support
		pipeline = std::make_unique<Pipeline>(
			p_device,
			p_render_pass,
			p_swapchain,
			vert,
			frag,
			&desc_layout,  // Enable descriptor sets
			true           // Enable vertex input
		);
	}

	void UIRenderer::createBuffers()
	{
		createQuadGeometry();
	}

	void UIRenderer::createQuadGeometry()
	{
		// Create a single unit quad vertices
		vec<UIVertex> vertices = {
			{{0.0f, 0.0f}, {0.0f, 0.0f}},  // Bottom-left
			{{1.0f, 0.0f}, {1.0f, 0.0f}},  // Bottom-right
			{{1.0f, 1.0f}, {1.0f, 1.0f}},  // Top-right
			{{0.0f, 1.0f}, {0.0f, 1.0f}}   // Top-left
		};

		vec<uint16_t> indices = {
			0, 1, 2,  // First triangle
			2, 3, 0   // Second triangle
		};

		// Create vertex buffer using VMA
		vk::DeviceSize vertex_size = sizeof(UIVertex) * vertices.size();
		vertex_buffer = createBuffer(
			vertex_size,
			vk::BufferUsageFlagBits::eVertexBuffer,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
			vertex_allocation
		);

		VmaAllocationInfo vertex_alloc_info;
		vmaGetAllocationInfo(p_device->allocator, vertex_allocation, &vertex_alloc_info);
		memcpy(vertex_alloc_info.pMappedData, vertices.data(), vertex_size);

		// Create index buffer using VMA
		vk::DeviceSize index_size = sizeof(uint16_t) * indices.size();
		index_buffer = createBuffer(
			index_size,
			vk::BufferUsageFlagBits::eIndexBuffer,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
			index_allocation
		);

		VmaAllocationInfo index_alloc_info;
		vmaGetAllocationInfo(p_device->allocator, index_allocation, &index_alloc_info);
		memcpy(index_alloc_info.pMappedData, indices.data(), index_size);
	}

	void UIRenderer::prepareFrame(
		const vec<std::unique_ptr<Button>>& buttons,
		const Window& window,
		DescriptorPool& desc_pool,
		DescriptorSetLayout& desc_layout
	)
	{
		draw_list.clear();

		for (const auto& button : buttons)
		{
			Texture* tex = button->getCurrentTexture();
			if (!tex) continue;

			DescriptorSet desc_set(p_device, &desc_pool, &desc_layout, *tex);

			// Create quad vertices in pixel space
			const auto& bl = button->getBottomLeft();
			const auto& tr = button->getTopRight();

			vec<UIVertex> vertices = {
				{{bl.x, bl.y}, {0.0f, 1.0f}},  // Bottom-left (UV bottom)
				{{tr.x, bl.y}, {1.0f, 1.0f}},  // Bottom-right
				{{tr.x, tr.y}, {1.0f, 0.0f}},  // Top-right (UV top)
				{{bl.x, tr.y}, {0.0f, 0.0f}}   // Top-left
			};

			vec<uint16_t> indices = {0, 1, 2, 2, 3, 0};

			UIQuad quad;
			quad.vertices = vertices;
			quad.indices = indices;
			quad.descriptor_set = desc_set.handle;

			draw_list.push_back(std::move(quad));
		}
	}

	void UIRenderer::recordCommands(vk::CommandBuffer cmd_buffer, uint32_t window_width, uint32_t window_height)
	{
		if (!pipeline) return;

		cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->handle);

		auto proj_matrix = Projection::createOrthographic(
			static_cast<float>(window_width),
			static_cast<float>(window_height)
		);

		cmd_buffer.pushConstants(
			pipeline->layout,
			vk::ShaderStageFlagBits::eVertex,
			0,
			sizeof(proj_matrix),
			proj_matrix.data()
		);

		vk::DeviceSize offset = 0;
		cmd_buffer.bindVertexBuffers(0, { vertex_buffer }, { offset });
		cmd_buffer.bindIndexBuffer(index_buffer, 0, vk::IndexType::eUint16);

		for (const auto& quad : draw_list)
		{
			cmd_buffer.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				pipeline->layout,
				0,
				{ quad.descriptor_set },
				{}
			);

			cmd_buffer.drawIndexed(6, 1, 0, 0, 0);
		}
	}

	vk::Buffer UIRenderer::createBuffer(
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		VmaMemoryUsage memory_usage,
		VmaAllocationCreateFlags flags,
		VmaAllocation& allocation
	)
	{
		VkBufferCreateInfo buffer_info = {};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = size;
		buffer_info.usage = static_cast<VkBufferUsageFlags>(usage);
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo alloc_info = {};
		alloc_info.usage = memory_usage;
		alloc_info.flags = flags;

		VkBuffer buffer;
		if (vmaCreateBuffer(p_device->allocator, &buffer_info, &alloc_info, &buffer, &allocation, nullptr) != VK_SUCCESS)
			err("Failed to create buffer with VMA");

		return static_cast<vk::Buffer>(buffer);
	}
}
