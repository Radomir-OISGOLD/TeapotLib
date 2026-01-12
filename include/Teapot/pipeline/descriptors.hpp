
#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	struct DescriptorSetLayout
	{
		DescriptorSetLayout(Init* init);
		~DescriptorSetLayout();

		// Non-copyable and non-movable
		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout(DescriptorSetLayout&&) = delete;
		DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

		VkDescriptorSetLayout handle = VK_NULL_HANDLE;
		Device* p_device = nullptr;
	};

	struct DescriptorPool
	{
		DescriptorPool(Init* init, uint32_t max_sets);
		~DescriptorPool();

		// Non-copyable and non-movable
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;
		DescriptorPool(DescriptorPool&&) = delete;
		DescriptorPool& operator=(DescriptorPool&&) = delete;

		VkDescriptorPool handle = VK_NULL_HANDLE;
		Device* p_device = nullptr;
	};

	struct DescriptorSet
	{
		DescriptorSet(
			RenderData* render_data,
			Texture& texture
		);

		// Non-copyable but movable
		DescriptorSet(const DescriptorSet&) = delete;
		DescriptorSet& operator=(const DescriptorSet&) = delete;
		DescriptorSet(DescriptorSet&&) = default;
		DescriptorSet& operator=(DescriptorSet&&) = default;

		VkDescriptorSet handle = VK_NULL_HANDLE;
		// No destructor - descriptor sets are cleaned up with the pool
	};
}

