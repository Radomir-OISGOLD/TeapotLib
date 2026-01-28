#include "Teapot/swapchain.hpp"
#include "Teapot/device.hpp"
#include "Teapot/image.hpp"

namespace Teapot
{
	Swapchain::Swapchain(Device* p_device)
		: p_device(p_device)
	{
		vkb::SwapchainBuilder swapchain_builder{ p_device->vkb_device };
		auto swap_ret = swapchain_builder.build();
		if (!swap_ret)
			err("Failed to create swapchain: " + swap_ret.error().message());
		
		vkb_swapchain = swap_ret.value();
		swapchain = vkb_swapchain.swapchain;

		createImages();
		createSyncObjects();
	}

	Swapchain::~Swapchain()
	{
		if (p_device == nullptr) return;

		for (auto semaphore : available_semaphores)
			p_device->device.destroySemaphore(semaphore);
		for (auto semaphore : finished_semaphores)
			p_device->device.destroySemaphore(semaphore);
		for (auto fence : in_flight_fences)
			p_device->device.destroyFence(fence);

		// Images' destructors will be called automatically
		vkb::destroy_swapchain(vkb_swapchain);
	}

	void Swapchain::createImages()
	{
		auto swapchain_images_r = vkb_swapchain.get_images();
		if (!swapchain_images_r)
			err("Failed to get swapchain images: " + swapchain_images_r.error().message());

		auto vk_images = swapchain_images_r.value();

		images.reserve(vk_images.size());
		for (auto& image : vk_images)
			images.emplace_back(p_device, image, static_cast<vk::Format>(vkb_swapchain.image_format));
	}

	void Swapchain::createSyncObjects()
	{
		available_semaphores.resize(TEAPOT_DOUBLE_BUFFERING);
		finished_semaphores.resize(TEAPOT_DOUBLE_BUFFERING);
		in_flight_fences.resize(TEAPOT_DOUBLE_BUFFERING);

		vk::SemaphoreCreateInfo semaphore_info;
		vk::FenceCreateInfo fence_info = { vk::FenceCreateFlagBits::eSignaled };

		for (size_t i = 0; i < TEAPOT_DOUBLE_BUFFERING; i++) {
			available_semaphores[i] = p_device->device.createSemaphore(semaphore_info);
			finished_semaphores[i] = p_device->device.createSemaphore(semaphore_info);
			in_flight_fences[i] = p_device->device.createFence(fence_info);
		}
	}
}
