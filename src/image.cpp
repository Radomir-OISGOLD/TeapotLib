	
#include "Teapot/image.hpp"
#include "Teapot/device.hpp"
#include "Teapot/commandpool.hpp"
#include "Teapot/queue.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>
#include <cstring>

namespace Teapot
{

	Image::Image(Device* p_device, vk::Image vk_image, vk::Format format)
		: image(vk_image), p_device(p_device)
	{
		vk::ImageViewCreateInfo view_info;
		view_info.image = image;
		view_info.viewType = vk::ImageViewType::e2D;
		view_info.format = format;
		view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;
		view_info.components = { vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity };

		view = p_device->device.createImageView(view_info);
	}

	Image::~Image()
	{
		if (view && p_device)
			p_device->device.destroyImageView(view);
	}

	Image::Image(Image&& other) noexcept
		: image(other.image)
		, view(other.view)
		, p_device(other.p_device)
	{
		other.image = nullptr;
		other.view = nullptr;
		other.p_device = nullptr;
	}

	Image& Image::operator=(Image&& other) noexcept
	{
		if (this != &other)
		{
			if (view && p_device)
				p_device->device.destroyImageView(view);

			image = other.image;
			view = other.view;
			p_device = other.p_device;

			other.image = nullptr;
			other.view = nullptr;
			other.p_device = nullptr;
		}
		return *this;
	}


	Texture::Texture(Device* p_device, CommandPool* p_command_pool, Queue* p_graphics_queue,
		LoadedImage& loaded_image) :
		p_device(p_device)
	{
		vk::DeviceSize img_size = static_cast<vk::DeviceSize>(loaded_image.width) * static_cast<vk::DeviceSize>(loaded_image.height) * 4u;

		// Create staging buffer using VMA
		VkBuffer staging_buffer;
		VmaAllocation staging_allocation;

		VkBufferCreateInfo buffer_info = {};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = img_size;
		buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo alloc_info = {};
		alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
		alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VmaAllocationInfo staging_alloc_info;
		if (vmaCreateBuffer(p_device->allocator, &buffer_info, &alloc_info, &staging_buffer, &staging_allocation, &staging_alloc_info) != VK_SUCCESS)
			throw std::runtime_error("Failed to create staging buffer with VMA");

		// Copy image data to staging buffer
		std::memcpy(staging_alloc_info.pMappedData, loaded_image.pixels.data(), static_cast<size_t>(img_size));

		// Create texture image using VMA
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

		VmaAllocationCreateInfo image_alloc_info = {};
		image_alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
		image_alloc_info.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

		if (vmaCreateImage(p_device->allocator, &image_info, &image_alloc_info, &vk_image, &allocation, nullptr) != VK_SUCCESS)
			throw std::runtime_error("Failed to create image with VMA");

		// Create command buffer for one-time commands
		vk::CommandBufferAllocateInfo cmd_alloc_info;
		cmd_alloc_info.level = vk::CommandBufferLevel::ePrimary;
		cmd_alloc_info.commandPool = p_command_pool->handle;
		cmd_alloc_info.commandBufferCount = 1;

		vk::CommandBuffer command_buffer = p_device->device.allocateCommandBuffers(cmd_alloc_info)[0];

		vk::CommandBufferBeginInfo begin_info;
		begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		command_buffer.begin(begin_info);

		// Transition image layout from undefined to transfer destination
		vk::ImageMemoryBarrier barrier;
		barrier.oldLayout = vk::ImageLayout::eUndefined;
		barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = static_cast<vk::Image>(vk_image);
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = {};
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		command_buffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eTopOfPipe,
			vk::PipelineStageFlagBits::eTransfer,
			{},
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		// Copy buffer to image
		vk::BufferImageCopy region;
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = vk::Offset3D{0, 0, 0};
		region.imageExtent = vk::Extent3D{
			static_cast<uint32_t>(loaded_image.width),
			static_cast<uint32_t>(loaded_image.height),
			1
		};

		command_buffer.copyBufferToImage(
			static_cast<vk::Buffer>(staging_buffer),
			static_cast<vk::Image>(vk_image),
			vk::ImageLayout::eTransferDstOptimal,
			1,
			&region
		);

		// Transition image layout from transfer destination to shader read only
		barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
		barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		command_buffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eFragmentShader,
			{},
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		command_buffer.end();

		vk::SubmitInfo submit_info;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		p_graphics_queue->handle.submit({ submit_info }, {});
		p_graphics_queue->handle.waitIdle();

		p_device->device.freeCommandBuffers(p_command_pool->handle, { command_buffer });

		vmaDestroyBuffer(p_device->allocator, staging_buffer, staging_allocation);

		this->image = std::make_unique<Image>(p_device, static_cast<vk::Image>(vk_image), vk::Format::eR8G8B8A8Srgb);

		vk::SamplerCreateInfo sampler_info;
		sampler_info.magFilter = vk::Filter::eLinear;
		sampler_info.minFilter = vk::Filter::eLinear;
		sampler_info.addressModeU = vk::SamplerAddressMode::eRepeat;
		sampler_info.addressModeV = vk::SamplerAddressMode::eRepeat;
		sampler_info.addressModeW = vk::SamplerAddressMode::eRepeat;
		sampler_info.anisotropyEnable = true;
		sampler_info.maxAnisotropy = 16.0f;
		sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
		sampler_info.unnormalizedCoordinates = false;
		sampler_info.compareEnable = false;
		sampler_info.compareOp = vk::CompareOp::eAlways;
		sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		sampler_info.mipLodBias = 0.0f;
		sampler_info.minLod = 0.0f;
		sampler_info.maxLod = 0.0f;

		sampler = p_device->device.createSampler(sampler_info);
	}

	Texture::~Texture()
	{
		if (sampler && p_device)
			p_device->device.destroySampler(sampler);
		if (image && image->image && p_device && allocation)
			vmaDestroyImage(p_device->allocator, static_cast<VkImage>(image->image), allocation);
		// image unique_ptr (and its view) is destroyed automatically
	}

	LoadedImage::LoadedImage(const char* path)
	{
		int req_channels = STBI_rgb_alpha;
		unsigned char* data = stbi_load(path, &width, &height, &channels, req_channels);

		if (!data)
			throw std::runtime_error(std::string("Failed to load image: ") + path);

		size_t pixel_count = static_cast<size_t>(width) * static_cast<size_t>(height) * 4u;
		pixels.resize(pixel_count);
		std::memcpy(pixels.data(), data, pixel_count);

		stbi_image_free(data);
	}
}

