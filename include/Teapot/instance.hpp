
#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	struct Instance
	{
		Instance(const char* app_name);
		~Instance();

		// Movable but not copyable
		Instance(const Instance&) = delete;
		Instance& operator=(const Instance&) = delete;

		// Factory methods
		Window* createWindow(const char* title, glm::uvec2 size);
		PhysDevice* createPhysDevice(Surface* p_surface);

		vk::Instance instance;
		vk::SurfaceKHR surface;
		vkb::Instance vkb_instance;

		vec<std::unique_ptr<Window>> windows;
		vec<std::unique_ptr<PhysDevice>> physical_devices;
	};
}
