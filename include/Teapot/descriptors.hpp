
#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	struct DescriptorSetLayout
	{
		DescriptorSetLayout(Device* p_device);
		~DescriptorSetLayout();

		// Non-copyable and non-movable
		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

		vk::DescriptorSetLayout handle;
		Device* p_device = nullptr;
	};

	struct DescriptorPool
	{
		DescriptorPool(Device* p_device, uint32_t max_sets);
		~DescriptorPool();

		// Non-copyable and non-movable
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

		vk::DescriptorPool handle;
		Device* p_device = nullptr;
	};

	struct DescriptorSet
	{
		DescriptorSet(
			Device* p_device,
			DescriptorPool* p_descriptor_pool,
			DescriptorSetLayout* p_descriptor_set_layout,
			Texture& texture
		);

		// Non-copyable but movable
		DescriptorSet(const DescriptorSet&) = delete;
		DescriptorSet& operator=(const DescriptorSet&) = delete;

		vk::DescriptorSet handle;
		// No destructor - descriptor sets are cleaned up with the pool
	};
}

