
#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	struct Surface;
	struct Instance;

	struct Window
	{

		Window(const char* title, u32t w, u32t h);

		~Window();

		GLFWwindow* handle;

		Surface* p_surface;

		Instance* p_instance;
	};

	struct Surface
	{

		Surface();
		
		~Surface();
		

		VkSurfaceKHR handle;

		Window* p_window;
	};

}

