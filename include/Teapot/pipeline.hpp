#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	class Pipeline
	{
	public:
		// Original constructor for basic pipelines
		Pipeline(Device* p_device, RenderPass* p_render_pass, Swapchain* p_swapchain, Shader& sh_vert, Shader& sh_frag);

		// UI pipeline constructor with descriptor sets and vertex input
		Pipeline(
			Device* p_device,
			RenderPass* p_render_pass,
			Swapchain* p_swapchain,
			Shader& sh_vert,
			Shader& sh_frag,
			DescriptorSetLayout* desc_layout,  // Optional descriptor set layout
			bool enable_vertex_input           // Enable vertex input bindings
		);

		~Pipeline();

		// Movable but not copyable
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		vk::Pipeline handle;
		vk::PipelineLayout layout;

		Device* p_device = nullptr; // non-owning

		struct PushConstants
		{
			float transform[16];  // 4x4 projection matrix
		};
	};
}
