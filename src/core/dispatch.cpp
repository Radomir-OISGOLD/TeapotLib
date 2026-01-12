#include "Teapot/core/dispatch.hpp"
#include "Teapot/core/device.hpp"
#include "Teapot/core/instance.hpp"
#include "Teapot/common/structures.hpp"

namespace Teapot
{
	DispatchTable::DispatchTable(Init* init)
	{
		handle = init->p_device->handle.make_table();
	}

	InstanceDispatchTable::InstanceDispatchTable(Init* init)
	{
		handle = init->p_instance->handle.make_table();
	}
}
