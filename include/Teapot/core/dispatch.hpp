#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	struct DispatchTable
	{
		DispatchTable(Init* init);
		~DispatchTable() = default;

		vkb::DispatchTable handle;
	};

	struct InstanceDispatchTable
	{
		InstanceDispatchTable(Init* init);
		~InstanceDispatchTable() = default;

		vkb::InstanceDispatchTable handle;
	};
}
