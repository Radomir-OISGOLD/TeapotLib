
#pragma once

#include <vulkan/vulkan.hpp>
#include <cstddef>

namespace Teapot
{
	struct UIVertex
	{
		float pos[2];  // x, y in pixels
		float uv[2];   // texture coordinates (0-1)

		static vk::VertexInputBindingDescription getBindingDescription()
		{
			vk::VertexInputBindingDescription binding;
			binding.binding = 0;
			binding.stride = sizeof(UIVertex);
			binding.inputRate = vk::VertexInputRate::eVertex;
			return binding;
		}

		static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions()
		{
			std::array<vk::VertexInputAttributeDescription, 2> attrs;

			// Position attribute (location = 0)
			attrs[0].binding = 0;
			attrs[0].location = 0;
			attrs[0].format = vk::Format::eR32G32Sfloat;
			attrs[0].offset = offsetof(UIVertex, pos);

			// UV attribute (location = 1)
			attrs[1].binding = 0;
			attrs[1].location = 1;
			attrs[1].format = vk::Format::eR32G32Sfloat;
			attrs[1].offset = offsetof(UIVertex, uv);

			return attrs;
		}
	};
}

