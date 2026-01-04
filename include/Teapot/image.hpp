
#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{

	struct Swapchain;

	struct Image
	{
		Image(VkImage handle, VkImageView view);

		~Image();

		VkImage handle;
		VkImageView view;

	};

	struct Texture
	{
		Texture(const char* file_path, Device& device, DispatchTable& table);

		Image image;

		VkDeviceMemory memory;
		VkSampler sampler;
	};

	struct LoadedImage 
	{
		LoadedImage(const char* path);

		int width;
		int height;
		int channels;
		std::vector<uint8_t> pixels;
	};

	


}
