
#include "Teapot/core/device.hpp"
#include "Teapot/core/instance.hpp"
#include "Teapot/platform/window.hpp"
#include "Teapot/core/dispatch.hpp"
#include "Teapot/core/swapchain.hpp"

#include <iostream>

namespace Teapot
{
	// --- PhysDevice ---
	PhysDevice::PhysDevice(Instance& inst, Surface& surface)
		: p_instance(&inst)
	{
		vkb::PhysicalDeviceSelector selector{ inst.handle };
		auto phys_ret = selector.set_surface(surface.handle)
			.set_minimum_version(1, 1) // Sensible default
			.select();

		if (!phys_ret) {
			err("Failed to select Vulkan Physical Device: " + phys_ret.error().message());
		}
		handle = phys_ret.value();
		std::cout << "Selected Vulkan Physical Device: " << handle.name << "\n";
	}

	std::unique_ptr<Device> PhysDevice::createLogicalDevice()
	{
		return std::make_unique<Device>(*this);
	}


	// --- Device ---
	Device::Device(PhysDevice& phys)
		: p_phys_device(&phys)
	{
		vkb::DeviceBuilder device_builder{ phys.handle };

		auto dev_ret = device_builder.build();
		if (!dev_ret) {
			err("Failed to create Vulkan logical device: " + dev_ret.error().message());
		}
		handle = dev_ret.value();

		table = std::make_unique<DispatchTable>(*this);
	}

	Device::~Device()
	{
		// swapchain and table are destroyed automatically by unique_ptr
		if (handle.device) {
			vkb::destroy_device(handle);
		}
	}
}
