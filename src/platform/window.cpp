
#include "Teapot/platform/window.hpp"
#include "Teapot/core/instance.hpp"

#include <stdexcept>

namespace Teapot
{
	// --- Window ---
	Window::Window(const char* title, unsigned int w, unsigned int h)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		handle = glfwCreateWindow(w, h, title, NULL, NULL);
		if (!handle) {
			err("Failed to create GLFW window.");
		}
	}

	Window::~Window()
	{
		if (handle) {
			glfwDestroyWindow(handle);
		}
	}

	void Window::createSurface(Instance& instance)
	{
		surface = std::make_unique<Surface>(*this, instance);
	}


	// --- Surface ---
	Surface::Surface(Window& window, Instance& instance) :
		p_window(&window),
		p_instance(&instance)
	{
		VkResult result = glfwCreateWindowSurface(instance.handle, window.handle, nullptr, &handle);
		if (result != VK_SUCCESS) {
			err("Failed to create window surface. Error: " + std::to_string(result));
		}
	}

	Surface::~Surface()
	{
		if (handle != VK_NULL_HANDLE && p_instance != nullptr) {
			vkb::destroy_surface(p_instance->handle, handle);
		}
	}
}
