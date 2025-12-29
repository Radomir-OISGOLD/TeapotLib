
#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	struct Surface;
	struct Instance;

	struct Window
	{
		Window(Instance& instance, const char* title, unsigned int w, unsigned int h);

		~Window();

		GLFWwindow* handle;

		Surface* p_surface;

		Instance* p_instance;
	};

	struct Surface
	{
		Surface(Window& window);
		
		~Surface();

		VkSurfaceKHR handle;

		Window* p_window;
	};
	

}

