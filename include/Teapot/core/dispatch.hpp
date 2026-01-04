#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	struct DispatchTable
	{
		DispatchTable(Device& device);
		~DispatchTable() = default;

		vkb::DispatchTable handle;
	};

	struct InstanceDispatchTable
	{
		InstanceDispatchTable(Instance& instance);
		~InstanceDispatchTable() = default;
		
		vkb::InstanceDispatchTable handle;
	};
}
