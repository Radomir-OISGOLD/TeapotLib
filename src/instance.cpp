

#include "Teapot/instance.hpp"

namespace Teapot
{

	Instance::Instance(const char* app_name)
	{
		vkb::InstanceBuilder builder;
		
		printf(std::to_string(glfwVulkanSupported()).c_str());
	
		u32t count = 0;
		vec<const char*> extensions;
		const char** glfw_exts = glfwGetRequiredInstanceExtensions(&count);
		extensions.resize(count);
		printf(std::to_string(count).c_str());
		for (u32t i = 0; i < count; i++)
		{
			extensions[i] = glfw_exts[i];
			printf("Enabling GLFW Extension: %s\n", extensions[i]);
		}
		auto inst_ret = builder.set_app_name(app_name).use_default_debug_messenger().request_validation_layers().build();

		if (!inst_ret) {
			err("Failed to create Vulkan instance.");
			delete this;
			return;
		}

		handle = inst_ret.value();
	}

	Instance::~Instance()
	{
		vkb::destroy_instance(handle);
	}

}

