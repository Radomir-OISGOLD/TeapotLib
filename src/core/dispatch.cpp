#include "Teapot/core/dispatch.hpp"
#include "Teapot/core/device.hpp"
#include "Teapot/core/instance.hpp"

namespace Teapot
{
	DispatchTable::DispatchTable(Device& device)
	{
		handle = device.handle.make_table();
	}

	InstanceDispatchTable::InstanceDispatchTable(Instance& instance)
	{
		handle = instance.handle.make_table();
	}
}
