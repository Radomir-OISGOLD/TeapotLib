#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	// Represents a non-owned VkImage and an owned VkImageView
	struct Image
	{
		Image(Init* init, VkImage image, VkFormat format);
		~Image();

		// Movable but not copyable
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;
		Image(Image&&) = default;
		Image& operator=(Image&&) = default;

		VkImage image = VK_NULL_HANDLE; // Non-owned
		VkImageView view = VK_NULL_HANDLE; // Owned

		Device* p_device = nullptr; // Non-owning, for cleanup
	};

	struct Texture
	{
		Texture(Init* init, RenderData* render_data, LoadedImage& loaded_image);
		~Texture();
		
		// Movable but not copyable
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture(Texture&&) = default;
		Texture& operator=(Texture&&) = default;

		std::unique_ptr<Image> image;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkSampler sampler = VK_NULL_HANDLE;
		
		Device* p_device = nullptr; // Non-owning, for cleanup
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
