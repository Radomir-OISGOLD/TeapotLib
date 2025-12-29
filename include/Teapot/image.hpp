
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

}
