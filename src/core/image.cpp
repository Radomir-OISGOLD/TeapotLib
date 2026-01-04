
#include "Teapot/core/image.hpp"
#include "Teapot/core/device.hpp"
#include "Teapot/core/dispatch.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <stdexcept>

namespace Teapot
{
	// --- Image ---
	Image::Image(Device& device, VkImage vk_image, VkFormat format)
		: image(vk_image), p_device(&device)
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
	Texture::Texture(LoadedImage& image, Device& device, 
		CommandPool& pool, Queue& graphics_queue) :
		p_device(&device)
	{
		VkDeviceSize img_size = image.width * image.height * 4;

		VkBuffer staging_buffer;
		VkDeviceMemory staging_memory;
		vkCreateBuffer(device.handle, /*...*/, &staging_buffer);

		
	}

	Texture::~Texture()
	{
		if (sampler != VK_NULL_HANDLE && p_device != nullptr) {
			p_device->table->handle.destroySampler(sampler, nullptr);
		}
		if (memory != VK_NULL_HANDLE && p_device != nullptr) {
			p_device->table->handle.freeMemory(memory, nullptr);
		}
		// image unique_ptr is destroyed automatically
	}

	// --- LoadedImage ---
	LoadedImage::LoadedImage(const char* path)
	{
		stbi_uc* data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
		if (!data)
			throw std::runtime_error("Failed to load image");

		channels = 4;
		pixels.resize(width * height * 4);
		memcpy(img.pixels.data(), data, img.pixels.size());

		stbi_image_free(data);
	}
}

