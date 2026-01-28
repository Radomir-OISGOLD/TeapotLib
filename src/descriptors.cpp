#include "Teapot/descriptors.hpp"
#include "Teapot/device.hpp"
#include "Teapot/image.hpp"

namespace Teapot
{
	DescriptorSetLayout::DescriptorSetLayout(Device* p_device)
		: p_device(p_device)
	{
		// Create binding for combined image sampler (texture + sampler)
		vk::DescriptorSetLayoutBinding sampler_binding;
		sampler_binding.binding = 0;
		sampler_binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		sampler_binding.descriptorCount = 1;
		sampler_binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
		sampler_binding.pImmutableSamplers = nullptr;

		vk::DescriptorSetLayoutCreateInfo layout_info;
		layout_info.bindingCount = 1;
		layout_info.pBindings = &sampler_binding;

		handle = p_device->device.createDescriptorSetLayout(layout_info);
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		if (handle && p_device)
			p_device->device.destroyDescriptorSetLayout(handle);
	}

	DescriptorPool::DescriptorPool(Device* p_device, uint32_t max_sets)
		: p_device(p_device)
	{
		vk::DescriptorPoolSize pool_size;
		pool_size.type = vk::DescriptorType::eCombinedImageSampler;
		pool_size.descriptorCount = max_sets;

		vk::DescriptorPoolCreateInfo pool_info;
		pool_info.poolSizeCount = 1;
		pool_info.pPoolSizes = &pool_size;
		pool_info.maxSets = max_sets;

		handle = p_device->device.createDescriptorPool(pool_info);
	}

	DescriptorPool::~DescriptorPool()
	{
		if (handle && p_device)
			p_device->device.destroyDescriptorPool(handle);
	}

	DescriptorSet::DescriptorSet(
		Device* p_device,
		DescriptorPool* p_descriptor_pool,
		DescriptorSetLayout* p_descriptor_set_layout,
		Texture& texture
	)
	{
		// Allocate descriptor set from pool
		vk::DescriptorSetAllocateInfo alloc_info;
		alloc_info.descriptorPool = p_descriptor_pool->handle;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &p_descriptor_set_layout->handle;

		handle = p_device->device.allocateDescriptorSets(alloc_info)[0];

		// Write texture to descriptor set
		vk::DescriptorImageInfo image_info;
		image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		image_info.imageView = texture.image->view;
		image_info.sampler = texture.sampler;

		vk::WriteDescriptorSet descriptor_write;
		descriptor_write.dstSet = handle;
		descriptor_write.dstBinding = 0;
		descriptor_write.dstArrayElement = 0;
		descriptor_write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		descriptor_write.descriptorCount = 1;
		descriptor_write.pImageInfo = &image_info;

		p_device->device.updateDescriptorSets({ descriptor_write }, {});
	}
}
