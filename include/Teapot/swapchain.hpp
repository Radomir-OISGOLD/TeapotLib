#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	struct Swapchain
	{
		friend struct Device;

		~Swapchain();

		// Movable but not copyable
		Swapchain(const Swapchain&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;

		vk::SwapchainKHR swapchain;
		vkb::Swapchain vkb_swapchain;
		vec<Image> images;

		Device* p_device = nullptr; // Non-owning

		// Sync structures
		vec<vk::Semaphore> available_semaphores;
		vec<vk::Semaphore> finished_semaphores;
		vec<vk::Fence> in_flight_fences;

	private:
		Swapchain(Device* p_device);
		void createImages();
		void createSyncObjects();
	};
}
