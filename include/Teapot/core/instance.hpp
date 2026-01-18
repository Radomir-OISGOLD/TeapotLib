
#pragma once

#include "Teapot/common/cap.hpp"

struct Init;

namespace Teapot
{
	struct Instance
	{
		Instance(const char* app_name, Init& init);
		~Instance();

		// Movable but not copyable
		Instance(const Instance&) = delete;
		Instance& operator=(const Instance&) = delete;
		Instance(Instance&&) = default;
		Instance& operator=(Instance&&) = default;


		vkb::Instance handle;
		
		vec<std::unique_ptr<Window>> windows;
		vec<std::unique_ptr<PhysDevice>> physical_devices;


	};
}
