#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	struct Queue
	{
		Queue(Device& device, vkb::QueueType type);
		~Queue() = default; // The queue is owned by the device, no need to destroy

		vk::Queue handle;
	};
}
