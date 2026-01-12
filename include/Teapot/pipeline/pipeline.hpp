#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	class Pipeline
	{
	public:
		// Original constructor for basic pipelines
		Pipeline(RenderData* render_data, Shader& sh_vert, Shader& sh_frag);

		// UI pipeline constructor with descriptor sets and vertex input
		Pipeline(
			RenderData* render_data,
			Shader& sh_vert,
			Shader& sh_frag,
			DescriptorSetLayout* desc_layout,  // Optional descriptor set layout
			bool enable_vertex_input           // Enable vertex input bindings
		);

		~Pipeline();

		// Movable but not copyable
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;
		Pipeline(Pipeline&&) = default;
		Pipeline& operator=(Pipeline&&) = default;

		VkPipeline handle = VK_NULL_HANDLE;
		VkPipelineLayout layout = VK_NULL_HANDLE;

		Device* p_device = nullptr; // non-owning

		struct PushConstants
		{
			float transform[16];  // 4x4 projection matrix
		};
	};
}
