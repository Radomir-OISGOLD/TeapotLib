
#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	class Projection
	{
	public:
		// Create orthographic projection matrix for pixel coordinates
		// Maps pixel space (0,0) at top-left to NDC
		static std::array<float, 16> createOrthographic(
			float width,   // Window width in pixels
			float height   // Window height in pixels
		)
		{
			// Orthographic projection: pixels → NDC
			// X: [0, width] → [-1, 1]
			// Y: [0, height] → [1, -1]  (flip Y axis)

			std::array<float, 16> matrix = {};
			matrix[0]  = 2.0f / width;   // Scale X
			matrix[5]  = -2.0f / height; // Scale Y (flip)
			matrix[10] = 1.0f;           // Z (identity)
			matrix[12] = -1.0f;          // Translate X
			matrix[13] = 1.0f;           // Translate Y
			matrix[15] = 1.0f;           // W

			return matrix;
		}
	};
}

