#include "Teapot/queue.hpp"
#include "Teapot/device.hpp"

namespace Teapot
{
	Queue::Queue(Device* p_device, vkb::QueueType type)
	{
		auto queue_ret = p_device->vkb_device.get_queue(type);
		if (!queue_ret)
			err("Failed to get queue: " + queue_ret.error().message());
		handle = queue_ret.value();
	}
}
