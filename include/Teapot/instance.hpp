
#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{

	struct PhysDevice;
	struct Window;

	struct Instance
	{

		Instance(const char* app_name);

		~Instance();

		vkb::Instance handle;

		vec<PhysDevice*> devices;
		vec<Window*> windows;
	};

	vec<Instance> g_instances;

}
