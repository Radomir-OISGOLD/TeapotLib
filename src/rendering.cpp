
#include "Teapot/rendering.hpp"
#include "Teapot/device.hpp"

namespace Teapot
{
	Queue::Queue(Device device, vkb::QueueType type)
	{
		auto queue_ret = device.handle.get_queue(type);
		if (!queue_ret) {
			std::cerr << "Failed to get queue. Error: " << queue_ret.error().message() << "\n";
			return;
		}
		handle = queue_ret.value();
	}

	Queue::~Queue()
	{
		delete this;
	}

	Swapchain::Swapchain(Device device)
	{
		vkb::SwapchainBuilder swapchain_builder{ device.handle };
		auto swap_ret = swapchain_builder.build();
		if (!swap_ret) {
			std::cout << swap_ret.error().message() << "\n";
			delete this;
			return;
		}
		handle = swap_ret.value();
	}

	Swapchain::~Swapchain()
	{
		vkb::destroy_swapchain(handle);
	}
}

