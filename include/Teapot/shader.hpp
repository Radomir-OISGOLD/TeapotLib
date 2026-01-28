#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	struct Shader
	{
		Shader(Device* p_device, const char* file_path);
		~Shader();

		vk::ShaderModule handle;
		Device* p_device = nullptr; // Non-owning
	};
}
