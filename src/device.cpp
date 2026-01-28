
#include "Teapot/device.hpp"
#include "Teapot/instance.hpp"
#include "Teapot/window.hpp"
#include "Teapot/swapchain.hpp"
#include "Teapot/queue.hpp"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include <iostream>

namespace Teapot
{
	PhysDevice::PhysDevice(Instance* p_instance)
		: p_instance(p_instance)
	{
		vkb::PhysicalDeviceSelector selector{ p_instance->vkb_instance };
		auto phys_ret = selector.set_surface(p_instance->surface)
			.set_minimum_version(1, 1)
			.select();
		if (!phys_ret)
			err("Failed to select Vulkan Physical Device: " + phys_ret.error().message());

		vkb_physical_device = phys_ret.value();
		physical_device = vkb_physical_device.physical_device;
		std::cout << "Selected Vulkan Physical Device: " << vkb_physical_device.name << "\n";
	}

	std::unique_ptr<Device> PhysDevice::createLogicalDevice()
	{
		// new bc sdt::make_unique can't access private constructor.
		return std::unique_ptr<Device>(new Device(this));
	}


	Device::Device(PhysDevice* p_phys_device)
		: p_phys_device(p_phys_device)
	{
		vkb::DeviceBuilder device_builder{ p_phys_device->vkb_physical_device };

		auto dev_ret = device_builder.build();
		if (!dev_ret)
			err("Failed to create Vulkan logical device: " + dev_ret.error().message());

		vkb_device = dev_ret.value();
		device = vkb_device.device;

		// Initialize VMA
		VmaVulkanFunctions vma_vulkan_func{};
		vma_vulkan_func.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
		vma_vulkan_func.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo allocator_info{};
		allocator_info.vulkanApiVersion = VK_API_VERSION_1_1;
		allocator_info.instance = p_phys_device->p_instance->instance;
		allocator_info.physicalDevice = p_phys_device->physical_device;
		allocator_info.device = device;
		allocator_info.pVulkanFunctions = &vma_vulkan_func;

		if (vmaCreateAllocator(&allocator_info, &allocator) != VK_SUCCESS)
			err("Failed to create VMA allocator");
	}

	Device::~Device()
	{
		// swapchain and queues are destroyed automatically by unique_ptr
		if (allocator)
			vmaDestroyAllocator(allocator);
		if (device)
			vkb::destroy_device(vkb_device);
	}

	Swapchain* Device::createSwapchain()
	{
		swapchain = std::unique_ptr<Swapchain>(new Swapchain(this));
		return swapchain.get();
	}

	Queue* Device::createQueue(vkb::QueueType type)
	{
		auto queue = std::unique_ptr<Queue>(new Queue(this, type));
		Queue* ptr = queue.get();
		queues.push_back(std::move(queue));
		return ptr;
	}
}
