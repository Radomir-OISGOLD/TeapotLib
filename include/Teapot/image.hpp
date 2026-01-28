#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{

	struct Image
	{
		Image(Device* p_device, vk::Image image, vk::Format format);
		~Image();

		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;

		Image(Image&& other) noexcept;
		Image& operator=(Image&& other) noexcept;

		vk::Image image;
		vk::ImageView view;

		Device* p_device = nullptr;
	};

	struct Texture
	{
		Texture(Device* p_device, CommandPool* p_command_pool, Queue* p_graphics_queue,
			LoadedImage& loaded_image);
		~Texture();

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		std::unique_ptr<Image> image;

		VmaAllocation allocation = VK_NULL_HANDLE;
		vk::Sampler sampler;

		Device* p_device = nullptr;
	};

	struct LoadedImage 
	{
		LoadedImage(const char* path);

		int width = 0;
		int height = 0;
		int channels = 0;
		std::vector<uint8_t> pixels;
	};
}
