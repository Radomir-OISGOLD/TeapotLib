#include "Teapot/pipeline/descriptors.hpp"
#include "Teapot/core/device.hpp"
#include "Teapot/core/dispatch.hpp"
#include "Teapot/core/image.hpp"
#include "Teapot/common/structures.hpp"

namespace Teapot
{
	DescriptorSetLayout::DescriptorSetLayout(Init* init)
		: p_device(init->p_device)
	{
		// Create binding for combined image sampler (texture + sampler)
		VkDescriptorSetLayoutBinding sampler_binding = {};
		sampler_binding.binding = 0;
		sampler_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sampler_binding.descriptorCount = 1;
		sampler_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		sampler_binding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo layout_info = {};
		layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_info.bindingCount = 1;
		layout_info.pBindings = &sampler_binding;

		isVkOk(
			p_device->table->handle.createDescriptorSetLayout(&layout_info, nullptr, &handle),
			"Failed to create descriptor set layout"
		);
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		if (handle != VK_NULL_HANDLE && p_device != nullptr)
		{
			p_device->table->handle.destroyDescriptorSetLayout(handle, nullptr);
		}
	}

	DescriptorPool::DescriptorPool(Init* init, uint32_t max_sets)
		: p_device(init->p_device)
	{
		VkDescriptorPoolSize pool_size = {};
		pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_size.descriptorCount = max_sets;

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.poolSizeCount = 1;
		pool_info.pPoolSizes = &pool_size;
		pool_info.maxSets = max_sets;

		isVkOk(
			p_device->table->handle.createDescriptorPool(&pool_info, nullptr, &handle),
			"Failed to create descriptor pool"
		);
	}

	DescriptorPool::~DescriptorPool()
	{
		if (handle != VK_NULL_HANDLE && p_device != nullptr)
		{
			// This automatically frees all descriptor sets allocated from this pool
			p_device->table->handle.destroyDescriptorPool(handle, nullptr);
		}
	}

	DescriptorSet::DescriptorSet(
		RenderData* render_data,
		Texture& texture
	)
	{
		// Allocate descriptor set from pool
		VkDescriptorSetAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.descriptorPool = render_data->p_descriptor_pool->handle;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &render_data->p_descriptor_set_layout->handle;

		isVkOk(
			render_data->p_device->table->handle.allocateDescriptorSets(&alloc_info, &handle),
			"Failed to allocate descriptor set"
		);

		// Write texture to descriptor set
		VkDescriptorImageInfo image_info = {};
		image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info.imageView = texture.image->view;
		image_info.sampler = texture.sampler;

		VkWriteDescriptorSet descriptor_write = {};
		descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_write.dstSet = handle;
		descriptor_write.dstBinding = 0;
		descriptor_write.dstArrayElement = 0;
		descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_write.descriptorCount = 1;
		descriptor_write.pImageInfo = &image_info;

		render_data->p_device->table->handle.updateDescriptorSets(1, &descriptor_write, 0, nullptr);
	}
}

