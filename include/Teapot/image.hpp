
#pragma once

#include "Teapot/cap.hpp"
#include "Teapot/rendering.hpp"

namespace Teapot
{

	struct Swapchain;

	struct Image
	{

		Image(Swapchain& swapchain);

		~Image();

		vk::Image vk_handle;


	};

}
