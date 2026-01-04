#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	struct Swapchain
	{
		Swapchain(Device& device);
		~Swapchain();

		// Movable but not copyable
		Swapchain(const Swapchain&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;
		Swapchain(Swapchain&&) = default;
		Swapchain& operator=(Swapchain&&) = default;

		vkb::Swapchain handle;
		vec<Image> images;
		
		Device* p_device = nullptr; // Non-owning

		// Sync structures
		vec<VkSemaphore> available_semaphores;
        vec<VkSemaphore> finished_semaphores;
        vec<VkFence> in_flight_fences;

	private:
		void create_images();
		void create_sync_objects();
	};
}
