
#pragma once

#include "Teapot/common/cap.hpp"
#include <cstddef>

namespace Teapot
{
	struct UIVertex
	{
		float pos[2];  // x, y in pixels
		float uv[2];   // texture coordinates (0-1)

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription binding = {};
			binding.binding = 0;
			binding.stride = sizeof(UIVertex);
			binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return binding;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 2> attrs = {};

			// Position attribute (location = 0)
			attrs[0].binding = 0;
			attrs[0].location = 0;
			attrs[0].format = VK_FORMAT_R32G32_SFLOAT;
			attrs[0].offset = offsetof(UIVertex, pos);

			// UV attribute (location = 1)
			attrs[1].binding = 0;
			attrs[1].location = 1;
			attrs[1].format = VK_FORMAT_R32G32_SFLOAT;
			attrs[1].offset = offsetof(UIVertex, uv);

			return attrs;
		}
	};
}

