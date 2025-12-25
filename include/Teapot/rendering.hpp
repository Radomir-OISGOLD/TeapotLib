
#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{

	struct Device;
	struct Queue
	{
		Queue(Device device, vkb::QueueType type);

		~Queue();

		Device* p_device;

		vk::Queue handle;
	};


	struct Swapchain
	{
		Swapchain(Device device);

		~Swapchain();

		vkb::Swapchain handle;

		Device* p_device;
	};

}
