
#include <stdexcept>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <VkBootstrap.h>

#include "Teapot/instance.hpp"
#include "Teapot/window.hpp"
#include "Teapot/device.hpp"

namespace Teapot
{
	Instance::Instance(const char* app_name)
	{
		if (!glfwInit())
			err("Failed to initialize GLFW.");

		vkb::InstanceBuilder builder;
		auto inst_ret = builder.set_app_name(app_name)
			.request_validation_layers(true)
			.use_default_debug_messenger()
			.build();
		if (!inst_ret)
			err("Failed to create Vulkan instance: " + inst_ret.error().message());

		vkb_instance = inst_ret.value();
		instance = vkb_instance.instance;
	}

	Instance::~Instance()
	{
		if (instance)
			vkb::destroy_instance(vkb_instance);
		glfwTerminate();
	}

	Window* Instance::createWindow(const char* title, glm::uvec2 size)
	{
		auto window = std::unique_ptr<Window>(new Window(title, size, this));
		Window* ptr = window.get();
		windows.push_back(std::move(window));
		return ptr;
	}

	PhysDevice* Instance::createPhysDevice(Surface* p_surface)
	{
		// Store the surface on the instance for PhysDevice to use
		surface = p_surface->handle;

		auto phys_device = std::unique_ptr<PhysDevice>(new PhysDevice(this));
		PhysDevice* ptr = phys_device.get();
		physical_devices.push_back(std::move(phys_device));
		return ptr;
	}
}

