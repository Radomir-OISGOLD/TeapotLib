
#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	struct Surface
	{
		Surface(Window& window, Instance& instance);
		~Surface();

		// Non-copyable/movable
		Surface(const Surface&) = delete;
		Surface& operator=(const Surface&) = delete;
		Surface(Surface&&) = delete;
		Surface& operator=(Surface&&) = delete;

		VkSurfaceKHR handle = VK_NULL_HANDLE;

		// Non-owning pointers for cleanup
		Instance* p_instance = nullptr; 
		Window* p_window = nullptr;
	};

	struct Window
	{
		Window(const char* title, unsigned int w, unsigned int h);
		~Window();

		// Movable but not copyable
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = default;
		Window& operator=(Window&&) = default;

		void createSurface(Instance& instance);

		GLFWwindow* handle = nullptr;
		std::unique_ptr<Surface> surface;
	};
}

