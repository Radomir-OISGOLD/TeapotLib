
#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	struct Instance;

	struct Device
	{
		friend struct PhysDevice;

		~Device();

		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;

		Swapchain* createSwapchain();
		Queue* createQueue(vkb::QueueType type);

		vk::Device device;
		vkb::Device vkb_device;
		VmaAllocator allocator = VK_NULL_HANDLE;

		PhysDevice* p_phys_device = nullptr;

		std::unique_ptr<Swapchain> swapchain;
		vec<std::unique_ptr<Queue>> queues;

	private:
		Device(PhysDevice* p_phys_device);
	};


	struct PhysDevice
	{
		friend struct Instance;

		~PhysDevice() = default;

		PhysDevice(const PhysDevice&) = delete;
		PhysDevice& operator=(const PhysDevice&) = delete;

		std::unique_ptr<Device> createLogicalDevice();
		vk::PhysicalDevice physical_device;
		vkb::PhysicalDevice vkb_physical_device;

		Instance* p_instance = nullptr;

	private:
		PhysDevice(Instance* p_instance);
	};

}



