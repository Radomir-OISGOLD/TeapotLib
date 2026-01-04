#include "Teapot/pipeline/commandpool.hpp"
#include "Teapot/core/device.hpp"

namespace Teapot
{
	CommandPool::CommandPool(Device& device, vkb::QueueType queue_type)
		: p_device(&device)
	{
		VkCommandPoolCreateInfo pool_info = {};
    	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    	
		auto queue_index_ret = device.handle.get_queue_index(queue_type);
		if (!queue_index_ret) {
			err("Failed to get queue index for command pool");
		}
    	pool_info.queueFamilyIndex = queue_index_ret.value();
		pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Allow resetting individual command buffers

    	isVkOk(p_device->table->createCommandPool(&pool_info, nullptr, &handle), "Failed to create command pool");
	}

	CommandPool::~CommandPool()
	{
		if (handle != VK_NULL_HANDLE && p_device != nullptr) {
			// Command buffers are implicitly freed with the pool
			p_device->table->destroyCommandPool(handle, nullptr);
		}
	}
	
	// The allocBuffers function needs a major redesign to fit the new structure.
	void CommandPool::allocBuffers(Pipeline& pipeline)
	{
		// TODO: Re-implement this.
		// This function was tightly coupled with the old monolithic structure.
		// A proper implementation requires a redesign of the render loop itself.
	}
}
