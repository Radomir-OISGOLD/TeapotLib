#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	struct Shader
	{
		Shader(Init* init, const char* file_path);
		~Shader();

		VkShaderModule handle;
		Device* p_device = nullptr; // Non-owning
	};
}
