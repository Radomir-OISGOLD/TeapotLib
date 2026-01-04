#include "Teapot/core/queue.hpp"
#include "Teapot/core/device.hpp"

namespace Teapot
{
	Queue::Queue(Device& device, vkb::QueueType type)
	{
		auto queue_ret = device.handle.get_queue(type);
		if (!queue_ret) {
			err("Failed to get queue: " + queue_ret.error().message());
		}
		handle = queue_ret.value();
	}
}
