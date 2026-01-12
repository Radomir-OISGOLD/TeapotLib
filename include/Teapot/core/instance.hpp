
#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	struct Instance
	{
		Instance(const char* app_name);
		~Instance();

		// Movable but not copyable
		Instance(const Instance&) = delete;
		Instance& operator=(const Instance&) = delete;
		Instance(Instance&&) = default;
		Instance& operator=(Instance&&) = default;

		Window& createWindow(const char* title, unsigned int w, unsigned int h);
		PhysDevice& createPhysicalDevice(Init* init);

		vkb::Instance handle;
		
		vec<std::unique_ptr<Window>> windows;
		vec<std::unique_ptr<PhysDevice>> physical_devices;
	};
}
