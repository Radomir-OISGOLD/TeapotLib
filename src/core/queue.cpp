#include "Teapot/core/queue.hpp"
#include "Teapot/core/device.hpp"
#include "Teapot/common/structures.hpp"

namespace Teapot
{
	Queue::Queue(Init* init, vkb::QueueType type)
	{
		auto queue_ret = init->p_device->handle.get_queue(type);
		if (!queue_ret) {
			err("Failed to get queue: " + queue_ret.error().message());
		}
		handle = queue_ret.value();
	}
}
