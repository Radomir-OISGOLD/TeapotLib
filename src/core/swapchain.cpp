#include "Teapot/core/swapchain.hpp"
#include "Teapot/core/device.hpp"
#include "Teapot/core/image.hpp"
#include "Teapot/core/dispatch.hpp"
#include "Teapot/common/structures.hpp"

namespace Teapot
{
	Swapchain::Swapchain(Init* init)
		: p_init(init), p_device(init->p_device)
	{
		vkb::SwapchainBuilder swapchain_builder{ init->p_device->handle };
		auto swap_ret = swapchain_builder.build();
		if (!swap_ret) {
			err("Failed to create swapchain: " + swap_ret.error().message());
		}
		handle = swap_ret.value();

		create_images();
		create_sync_objects();
	}

	Swapchain::~Swapchain()
	{
		if (p_device == nullptr) return;

		for (auto semaphore : available_semaphores)
		{
			p_device->table->handle.destroySemaphore(semaphore, nullptr);
		}
		for (auto semaphore : finished_semaphores)
		{
			p_device->table->handle.destroySemaphore(semaphore, nullptr);
		}
		for (auto fence : in_flight_fences)
		{
			p_device->table->handle.destroyFence(fence, nullptr);
		}

		// Images' destructors will be called automatically
		vkb::destroy_swapchain(handle);
	}

	void Swapchain::create_images()
	{
		auto swapchain_images_r = handle.get_images();
		if (!swapchain_images_r) {
			err("Failed to get swapchain images: " + swapchain_images_r.error().message());
		}
		auto vk_images = swapchain_images_r.value();
		
		images.reserve(vk_images.size());
		for (auto& image : vk_images) {
			images.emplace_back(p_init, image, handle.image_format);
		}
	}

	void Swapchain::create_sync_objects()
	{
		available_semaphores.resize(TEAPOT_DOUBLE_BUFFERING);
		finished_semaphores.resize(TEAPOT_DOUBLE_BUFFERING);
		in_flight_fences.resize(TEAPOT_DOUBLE_BUFFERING);

		VkSemaphoreCreateInfo semaphore_info = {};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fence_info = {};
		fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < TEAPOT_DOUBLE_BUFFERING; i++) {
			isVkOk(p_device->table->handle.createSemaphore(&semaphore_info, nullptr, &available_semaphores[i]), "Failed to create available_semaphores");
			isVkOk(p_device->table->handle.createSemaphore(&semaphore_info, nullptr, &finished_semaphores[i]), "Failed to create finished_semaphores");
			isVkOk(p_device->table->handle.createFence(&fence_info, nullptr, &in_flight_fences[i]), "Failed to create in_flight_fences");
		}
	}
}
