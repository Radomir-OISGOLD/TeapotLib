#include "Teapot/shader.hpp"
#include "Teapot/device.hpp"

#include <fstream>
#include <vector>

namespace
{
	std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
	
		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}
	
		size_t file_size = (size_t)file.tellg();
		std::vector<char> buffer(file_size);
	
		file.seekg(0);
		file.read(buffer.data(), static_cast<std::streamsize>(file_size));
	
		file.close();
	
		return buffer;
	}
}

namespace Teapot
{
	Shader::Shader(Device* p_device, const char* file_path)
		: p_device(p_device)
	{
		auto code = readFile(file_path);

		vk::ShaderModuleCreateInfo create_info;
		create_info.codeSize = code.size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

		handle = p_device->device.createShaderModule(create_info);
	}

	Shader::~Shader()
	{
		if (handle && p_device)
			p_device->device.destroyShaderModule(handle);
	}
}
