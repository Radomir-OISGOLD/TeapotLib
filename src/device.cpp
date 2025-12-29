
#include "Teapot/device.hpp"

#include "Teapot/instance.hpp"
#include "Teapot/window.hpp"
#include "Teapot/rendering.hpp"

namespace Teapot
{
	PhysDevice::PhysDevice(Surface& surface)
	{
		vkb::PhysicalDeviceSelector selector{ p_instance->handle };
		auto phys_ret = selector.set_surface(surface.handle).select();
		if (!phys_ret) {
			std::cerr << "Failed to select Vulkan Physical Device. Error: " << phys_ret.error().message() << "\n";
			if (phys_ret.error() == vkb::PhysicalDeviceError::no_suitable_device) {
				const auto& detailed_reasons = phys_ret.detailed_failure_reasons();
				if (!detailed_reasons.empty()) {
					std::cerr << "GPU Selection failure reasons:\n";
					for (const std::string& reason : detailed_reasons) {
						std::cerr << reason << "\n";
					}
				}
			}
			delete this;
		}
	}

	Device::Device(PhysDevice& phys)
	{
		vkb::DeviceBuilder device_builder{ phys.handle };

		auto dev_ret = device_builder.build();
		if (!dev_ret) {
			std::cerr << "Failed to create Vulkan device. Error: " << dev_ret.error().message() << "\n";
			delete this;
			return;
		}
		handle = dev_ret.value();
		phys.log_devices.push_back(this);
	}

}
