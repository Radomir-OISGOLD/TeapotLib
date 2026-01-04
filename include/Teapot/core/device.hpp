
#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	struct Device
	{
		Device(PhysDevice& phys);
		~Device();
		
		// Movable but not copyable
		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;
		Device(Device&&) = default;
		Device& operator=(Device&&) = default;

		vkb::Device handle;
		
		// Non-owning pointer back to the physical device
		PhysDevice* p_phys_device = nullptr; 
		
		std::unique_ptr<Swapchain> swapchain;
		std::unique_ptr<DispatchTable> table;
	};

	struct PhysDevice
	{
		PhysDevice(Instance& inst, Surface& surface);
		~PhysDevice() = default; 

		// Movable but not copyable
		PhysDevice(const PhysDevice&) = delete;
		PhysDevice& operator=(const PhysDevice&) = delete;
		PhysDevice(PhysDevice&&) = default;
		PhysDevice& operator=(PhysDevice&&) = default;

		std::unique_ptr<Device> createLogicalDevice();

		vkb::PhysicalDevice handle;

		// Non-owning pointer back to the instance
		Instance* p_instance = nullptr;
	};
}

