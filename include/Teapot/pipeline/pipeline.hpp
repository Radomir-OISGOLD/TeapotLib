#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	class Pipeline
	{
	public:
		Pipeline(Device& device, RenderPass& render_pass, Swapchain& swapchain, Shader& sh_vert, Shader& sh_frag);
		~Pipeline();

		// Movable but not copyable
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;
		Pipeline(Pipeline&&) = default;
		Pipeline& operator=(Pipeline&&) = default;

		VkPipeline handle = VK_NULL_HANDLE;
		VkPipelineLayout layout = VK_NULL_HANDLE;

		Device* p_device = nullptr; // non-owning
	};
}
