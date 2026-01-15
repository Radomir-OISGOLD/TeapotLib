
#include "Teapot/core/device.hpp"
#include "Teapot/core/instance.hpp"
#include "Teapot/platform/window.hpp"
#include "Teapot/core/dispatch.hpp"
#include "Teapot/core/swapchain.hpp"
#include "Teapot/common/structures.hpp"

#include <iostream>

namespace Teapot
{
	// --- PhysDevice ---
	PhysDevice::PhysDevice(Init* init)
		: p_instance(init->p_instance)
	{
		vkb::PhysicalDeviceSelector selector{ init->p_instance->handle };
		auto phys_ret = selector.set_surface(init->p_surface->handle)
			.set_minimum_version(1, 1) // Sensible default
			.select();

		if (!phys_ret) {
			err("Failed to select Vulkan Physical Device: " + phys_ret.error().message());
		}
		handle = phys_ret.value();
		std::cout << "Selected Vulkan Physical Device: " << handle.name << "\n";
	}

	std::unique_ptr<Device> PhysDevice::createLogicalDevice(Init* init)
	{
		return std::make_unique<Device>(init);
	}


	// --- Device ---
	Device::Device(Init* init)
		: p_phys_device(init->p_phys_device)
	{
		vkb::DeviceBuilder device_builder{ init->p_phys_device->handle };

		auto dev_ret = device_builder.build();
		if (!dev_ret) {
			err("Failed to create Vulkan logical device: " + dev_ret.error().message());
		}
		handle = dev_ret.value();

		printf("is ok \n");
		// Update init->p_device before creating dispatch table
		table = std::make_unique<DispatchTable>(init);
	}

	Device::~Device()
	{
		// swapchain and table are destroyed automatically by unique_ptr
		if (handle.device) {
			vkb::destroy_device(handle);
		}
	}
}
