
#include "Teapot/core/instance.hpp"
#include "Teapot/platform/window.hpp"
#include "Teapot/core/device.hpp"

#include <stdexcept>

namespace Teapot
{
	Instance::Instance(const char* app_name)
	{
		if (!glfwInit()) {
			err("Failed to initialize GLFW.");
		}

		vkb::InstanceBuilder builder;
		
		uint32_t glfw_extension_count = 0;
		const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		builder.set_app_name(app_name)
			.request_validation_layers(true)
			.use_default_debug_messenger();
		
		for (uint32_t i = 0; i < glfw_extension_count; i++) {
			builder.enable_extension(glfw_extensions[i]);
		}

		auto inst_ret = builder.build();
		if (!inst_ret) {
			err("Failed to create Vulkan instance: " + inst_ret.error().message());
		}
		handle = inst_ret.value();
	}

	Instance::~Instance()
	{
		// physical_devices and windows are destroyed automatically by unique_ptr
		if (handle.instance) {
			vkb::destroy_instance(handle);
		}
		glfwTerminate();
	}

	Window& Instance::createWindow(const char* title, unsigned int w, unsigned int h)
	{
		windows.emplace_back(std::make_unique<Window>(title, w, h));
		return *windows.back();
	}

	PhysDevice& Instance::createPhysicalDevice(Surface& surface)
	{
		physical_devices.emplace_back(std::make_unique<PhysDevice>(*this, surface));
		return *physical_devices.back();
	}
}

