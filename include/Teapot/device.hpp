
#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	struct Surface;
	struct Device;
	struct Instance;
	struct Swapchain;

	struct PhysDevice
	{

		PhysDevice(Instance& inst, Surface& surface);

		vkb::PhysicalDevice handle;

		vec<Device*> log_devices;

		Instance* p_instance;
	};

	struct Device
	{

		Device(PhysDevice& phys);

		~Device()
		{
			vkb::destroy_device(handle);
		}

		vkb::Device handle;

		Swapchain* p_swapchain;

		PhysDevice* p_phys_device;
	};

}

