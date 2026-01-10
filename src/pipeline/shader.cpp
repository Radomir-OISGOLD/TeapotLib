#include "Teapot/pipeline/shader.hpp"
#include "Teapot/core/device.hpp"
#include "Teapot/core/dispatch.hpp"

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
	Shader::Shader(Device& device, const char* file_path)
		: p_device(&device)
	{
		auto code = readFile(file_path);

		VkShaderModuleCreateInfo create_info = {};
   		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    	create_info.codeSize = code.size();
    	create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    	isVkOk(p_device->table->handle.createShaderModule(&create_info, nullptr, &handle), "Failed to create shader module");
	}

	Shader::~Shader()
	{
		if (handle != VK_NULL_HANDLE && p_device != nullptr) {
			p_device->table->handle.destroyShaderModule(handle, nullptr);
		}
	}
}
