
#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	struct Surface;
	struct Instance;

	struct Window
	{
		Window(const char* title, unsigned int w, unsigned int h);

		~Window();

		GLFWwindow* handle;

		Surface* p_surface;

	};

	struct Surface
	{
		Surface(Window& window, Instance& instance);
		
		~Surface();

		VkSurfaceKHR handle;

		Window* p_window;
		Instance* p_instance;
	};
	

}

