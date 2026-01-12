#include "Teapot/ui/ui_renderer.hpp"
#include "Teapot/ui/button.hpp"
#include "Teapot/ui/projection.hpp"
#include "Teapot/core/device.hpp"
#include "Teapot/core/swapchain.hpp"
#include "Teapot/core/dispatch.hpp"
#include "Teapot/core/image.hpp"
#include "Teapot/pipeline/renderpass.hpp"
#include "Teapot/pipeline/pipeline.hpp"
#include "Teapot/pipeline/descriptors.hpp"
#include "Teapot/platform/window.hpp"
#include "Teapot/common/structures.hpp"

namespace Teapot
{
	UIRenderer::UIRenderer(RenderData* render_data)
		: p_render_data(render_data)
		, p_device(render_data->p_device)
		, p_render_pass(render_data->p_render_pass)
		, p_swapchain(render_data->p_swapchain)
	{
	}

	UIRenderer::~UIRenderer()
	{
		if (p_device == nullptr) return;

		if (vertex_buffer != VK_NULL_HANDLE)
		{
			p_device->table->handle.destroyBuffer(vertex_buffer, nullptr);
		}
		if (vertex_memory != VK_NULL_HANDLE)
		{
			p_device->table->handle.freeMemory(vertex_memory, nullptr);
		}
		if (index_buffer != VK_NULL_HANDLE)
		{
			p_device->table->handle.destroyBuffer(index_buffer, nullptr);
		}
		if (index_memory != VK_NULL_HANDLE)
		{
			p_device->table->handle.freeMemory(index_memory, nullptr);
		}
	}

	void UIRenderer::createPipeline(Shader& vert, Shader& frag, DescriptorSetLayout& desc_layout)
	{
		// Create pipeline with vertex input and descriptor set support
		pipeline = std::make_unique<Pipeline>(
			p_render_data,
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

		// Create vertex buffer
		VkDeviceSize vertex_size = sizeof(UIVertex) * vertices.size();
		vertex_buffer = createBuffer(
			vertex_size,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertex_memory
		);

		void* data;
		p_device->table->handle.mapMemory(vertex_memory, 0, vertex_size, 0, &data);
		memcpy(data, vertices.data(), vertex_size);
		p_device->table->handle.unmapMemory(vertex_memory);

		// Create index buffer
		VkDeviceSize index_size = sizeof(uint16_t) * indices.size();
		index_buffer = createBuffer(
			index_size,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			index_memory
		);

		p_device->table->handle.mapMemory(index_memory, 0, index_size, 0, &data);
		memcpy(data, indices.data(), index_size);
		p_device->table->handle.unmapMemory(index_memory);
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

			// Create descriptor set for this button's texture
			// Note: This creates a temporary RenderData-like structure for the descriptor set
			// In practice, you'd want to pass the actual RenderData* to this method
			// For now, keeping the old signature for desc_pool and desc_layout parameters
			RenderData temp_render_data;
			temp_render_data.p_device = p_device;
			temp_render_data.p_descriptor_pool = &desc_pool;
			temp_render_data.p_descriptor_set_layout = &desc_layout;
			DescriptorSet desc_set(&temp_render_data, *tex);

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

	void UIRenderer::recordCommands(VkCommandBuffer cmd_buffer, uint32_t window_width, uint32_t window_height)
	{
		if (!pipeline) return;

		// Bind pipeline
		p_device->table->handle.cmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->handle);

		// Push projection matrix
		auto proj_matrix = Projection::createOrthographic(
			static_cast<float>(window_width),
			static_cast<float>(window_height)
		);

		p_device->table->handle.cmdPushConstants(
			cmd_buffer,
			pipeline->layout,
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(proj_matrix),
			proj_matrix.data()
		);

		// Bind vertex and index buffers
		VkDeviceSize offset = 0;
		p_device->table->handle.cmdBindVertexBuffers(cmd_buffer, 0, 1, &vertex_buffer, &offset);
		p_device->table->handle.cmdBindIndexBuffer(cmd_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT16);

		// Draw each button
		for (const auto& quad : draw_list)
		{
			// Bind descriptor set for this button's texture
			p_device->table->handle.cmdBindDescriptorSets(
				cmd_buffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipeline->layout,
				0,
				1,
				&quad.descriptor_set,
				0,
				nullptr
			);

			// Draw indexed (6 indices for 2 triangles)
			p_device->table->handle.cmdDrawIndexed(cmd_buffer, 6, 1, 0, 0, 0);
		}
	}

	VkBuffer UIRenderer::createBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkDeviceMemory& memory
	)
	{
		VkBuffer buffer;

		VkBufferCreateInfo buffer_info = {};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		isVkOk(
			p_device->table->handle.createBuffer(&buffer_info, nullptr, &buffer),
			"Failed to create buffer"
		);

		VkMemoryRequirements mem_requirements;
		p_device->table->handle.getBufferMemoryRequirements(buffer, &mem_requirements);

		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirements.size;
		alloc_info.memoryTypeIndex = findMemoryType(mem_requirements.memoryTypeBits, properties);

		isVkOk(
			p_device->table->handle.allocateMemory(&alloc_info, nullptr, &memory),
			"Failed to allocate buffer memory"
		);

		p_device->table->handle.bindBufferMemory(buffer, memory, 0);

		return buffer;
	}

	uint32_t UIRenderer::findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties mem_properties;
		vkGetPhysicalDeviceMemoryProperties(p_device->handle.physical_device, &mem_properties);

		for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
		{
			if ((type_filter & (1 << i)) &&
				(mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		err("Failed to find suitable memory type");
		return 0;
	}
}

