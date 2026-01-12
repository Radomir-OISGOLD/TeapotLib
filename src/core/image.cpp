
#include "Teapot/core/image.hpp"
#include "Teapot/core/device.hpp"
#include "Teapot/core/dispatch.hpp"
#include "Teapot/pipeline/commandpool.hpp"
#include "Teapot/core/queue.hpp"
#include "Teapot/common/structures.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <stdexcept>

namespace Teapot
{
	// --- Image ---
	Image::Image(Init* init, VkImage vk_image, VkFormat format)
		: image(vk_image), p_device(init->p_device)
	{
		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = image;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = format;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		isVkOk(p_device->table->handle.createImageView(&view_info, nullptr, &view), "Failed to create image view");
	}

	Image::~Image()
	{
		if (view != VK_NULL_HANDLE && p_device != nullptr) {
			p_device->table->handle.destroyImageView(view, nullptr);
		}
	}


	// --- Texture ---
	Texture::Texture(Init* init, RenderData* render_data, LoadedImage& loaded_image) :
		p_device(init->p_device)
	{
		VkDeviceSize img_size = loaded_image.width * loaded_image.height * 4;

		// Create staging buffer
		VkBuffer staging_buffer;
		VkDeviceMemory staging_memory;

		VkBufferCreateInfo buffer_info = {};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = img_size;
		buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		isVkOk(init->p_device->table->handle.createBuffer(&buffer_info, nullptr, &staging_buffer),
			"Failed to create staging buffer");

		// Get memory requirements for staging buffer
		VkMemoryRequirements mem_requirements;
		init->p_device->table->handle.getBufferMemoryRequirements(staging_buffer, &mem_requirements);

		// Find suitable memory type (host visible and coherent)
		VkPhysicalDeviceMemoryProperties mem_properties;
		vkGetPhysicalDeviceMemoryProperties(init->p_phys_device->handle.physical_device, &mem_properties);

		uint32_t memory_type_index = UINT32_MAX;
		VkMemoryPropertyFlags required_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
			if ((mem_requirements.memoryTypeBits & (1 << i)) &&
				(mem_properties.memoryTypes[i].propertyFlags & required_properties) == required_properties) {
				memory_type_index = i;
				break;
			}
		}

		if (memory_type_index == UINT32_MAX)
			throw std::runtime_error("Failed to find suitable memory type for staging buffer");

		// Allocate staging buffer memory
		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirements.size;
		alloc_info.memoryTypeIndex = memory_type_index;

		isVkOk(init->p_device->table->handle.allocateMemory(&alloc_info, nullptr, &staging_memory),
			"Failed to allocate staging buffer memory");

		init->p_device->table->handle.bindBufferMemory(staging_buffer, staging_memory, 0);

		// Copy image data to staging buffer
		void* data;
		init->p_device->table->handle.mapMemory(staging_memory, 0, img_size, 0, &data);
		memcpy(data, loaded_image.pixels.data(), static_cast<size_t>(img_size));
		init->p_device->table->handle.unmapMemory(staging_memory);

		// Create texture image
		VkImage vk_image;
		VkImageCreateInfo image_info = {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent.width = static_cast<uint32_t>(loaded_image.width);
		image_info.extent.height = static_cast<uint32_t>(loaded_image.height);
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.format = VK_FORMAT_R8G8B8A8_SRGB;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.flags = 0;

		isVkOk(init->p_device->table->handle.createImage(&image_info, nullptr, &vk_image),
			"Failed to create texture image");

		// Get memory requirements for image
		init->p_device->table->handle.getImageMemoryRequirements(vk_image, &mem_requirements);

		// Find suitable memory type (device local)
		memory_type_index = UINT32_MAX;
		required_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
			if ((mem_requirements.memoryTypeBits & (1 << i)) &&
				(mem_properties.memoryTypes[i].propertyFlags & required_properties) == required_properties) {
				memory_type_index = i;
				break;
			}
		}

		if (memory_type_index == UINT32_MAX)
			throw std::runtime_error("Failed to find suitable memory type for texture image");

		// Allocate image memory
		alloc_info.allocationSize = mem_requirements.size;
		alloc_info.memoryTypeIndex = memory_type_index;

		isVkOk(init->p_device->table->handle.allocateMemory(&alloc_info, nullptr, &memory),
			"Failed to allocate texture image memory");

		init->p_device->table->handle.bindImageMemory(vk_image, memory, 0);

		// Create command buffer for one-time commands
		VkCommandBufferAllocateInfo cmd_alloc_info = {};
		cmd_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmd_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmd_alloc_info.commandPool = render_data->p_command_pool->handle;
		cmd_alloc_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		init->p_device->table->handle.allocateCommandBuffers(&cmd_alloc_info, &command_buffer);

		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		init->p_device->table->handle.beginCommandBuffer(command_buffer, &begin_info);

		// Transition image layout from undefined to transfer destination
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = vk_image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		init->p_device->table->handle.cmdPipelineBarrier(
			command_buffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		// Copy buffer to image
		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = {0, 0, 0};
		region.imageExtent = {
			static_cast<uint32_t>(loaded_image.width),
			static_cast<uint32_t>(loaded_image.height),
			1
		};

		init->p_device->table->handle.cmdCopyBufferToImage(
			command_buffer,
			staging_buffer,
			vk_image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		// Transition image layout from transfer destination to shader read only
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		init->p_device->table->handle.cmdPipelineBarrier(
			command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		init->p_device->table->handle.endCommandBuffer(command_buffer);

		// Submit command buffer and wait
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		isVkOk(render_data->p_graphics_queue->handle.submit(1, reinterpret_cast<vk::SubmitInfo*>(&submit_info), VK_NULL_HANDLE), "Failed to submit command buffer");
		render_data->p_graphics_queue->handle.waitIdle();

		init->p_device->table->handle.freeCommandBuffers(render_data->p_command_pool->handle, 1, &command_buffer);

		// Clean up staging resources
		init->p_device->table->handle.destroyBuffer(staging_buffer, nullptr);
		init->p_device->table->handle.freeMemory(staging_memory, nullptr);

		// Create image view using Image class
		this->image = std::make_unique<Image>(init, vk_image, VK_FORMAT_R8G8B8A8_SRGB);

		// Create sampler
		VkSamplerCreateInfo sampler_info = {};
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_info.magFilter = VK_FILTER_LINEAR;
		sampler_info.minFilter = VK_FILTER_LINEAR;
		sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.anisotropyEnable = VK_TRUE;
		sampler_info.maxAnisotropy = 16.0f;
		sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.compareEnable = VK_FALSE;
		sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler_info.mipLodBias = 0.0f;
		sampler_info.minLod = 0.0f;
		sampler_info.maxLod = 0.0f;

		isVkOk(init->p_device->table->handle.createSampler(&sampler_info, nullptr, &sampler),
			"Failed to create texture sampler");
	}

	Texture::~Texture()
	{
		if (sampler != VK_NULL_HANDLE && p_device != nullptr) {
			p_device->table->handle.destroySampler(sampler, nullptr);
		}
		if (image && image->image != VK_NULL_HANDLE && p_device != nullptr) {
			p_device->table->handle.destroyImage(image->image, nullptr);
		}
		if (memory != VK_NULL_HANDLE && p_device != nullptr) {
			p_device->table->handle.freeMemory(memory, nullptr);
		}
		// image unique_ptr (and its view) is destroyed automatically
	}

	// --- LoadedImage ---
	LoadedImage::LoadedImage(const char* path)
	{
		stbi_uc* data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
		if (!data)
			throw std::runtime_error("Failed to load image");

		channels = 4;
		pixels.resize(width * height * 4);
		memcpy(pixels.data(), data, pixels.size());

		stbi_image_free(data);
	}
}

