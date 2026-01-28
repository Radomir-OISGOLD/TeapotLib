#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	struct Queue
	{
		friend struct Device;

		~Queue() = default; // The queue is owned by the device, no need to destroy

		vk::Queue handle;

	private:
		Queue(Device* p_device, vkb::QueueType type);
	};
}
