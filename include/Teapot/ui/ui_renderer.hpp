
#pragma once

#include "Teapot/common/cap.hpp"
#include "Teapot/ui/vertex.hpp"

namespace Teapot
{
	struct UIQuad
	{
		vec<UIVertex> vertices;
		vec<uint16_t> indices;
		VkDescriptorSet descriptor_set;
	};

	class UIRenderer
	{
	public:
		UIRenderer(RenderData* render_data);
		~UIRenderer();

		// Non-copyable and non-movable
		UIRenderer(const UIRenderer&) = delete;
		UIRenderer& operator=(const UIRenderer&) = delete;
		UIRenderer(UIRenderer&&) = delete;
		UIRenderer& operator=(UIRenderer&&) = delete;

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
		void recordCommands(VkCommandBuffer cmd_buffer, uint32_t window_width, uint32_t window_height);

	private:
		RenderData* p_render_data = nullptr;
		Device* p_device = nullptr;
		RenderPass* p_render_pass = nullptr;
		Swapchain* p_swapchain = nullptr;

		std::unique_ptr<Pipeline> pipeline;

		VkBuffer vertex_buffer = VK_NULL_HANDLE;
		VkDeviceMemory vertex_memory = VK_NULL_HANDLE;
		VkBuffer index_buffer = VK_NULL_HANDLE;
		VkDeviceMemory index_memory = VK_NULL_HANDLE;

		vec<UIQuad> draw_list;

		void createQuadGeometry();
		VkBuffer createBuffer(
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkDeviceMemory& memory
		);
		uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);
	};
}

