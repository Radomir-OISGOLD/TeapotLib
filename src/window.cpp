
#include "Teapot/window.hpp"

#include "Teapot/instance.hpp"
#include <cstdint>

namespace Teapot
{
	Teapot::Window::Window(Instance& instance, const char* title, unsigned int w, unsigned int h) :
		p_instance(&instance)
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		handle = glfwCreateWindow(w, h, title, NULL, NULL);
		if (!handle) {
			err("Failed to create GLFW window.");
			delete this;
			return;
		}
		glfwMakeContextCurrent(handle);
	}

	Window::~Window()
	{
		glfwDestroyWindow(handle);
	}

	Surface::Surface(Window& window) :
		p_window(&window)
	{
		VkResult glfw_result = glfwCreateWindowSurface(window.p_instance->handle, window.handle, nullptr, &handle);
		if (glfw_result != VK_SUCCESS) {
			std::cerr << "Failed to select create window surface. Error: " << std::to_string(glfw_result) << "\n";
			delete this;
			return;
		}
		p_window->p_surface = this;
	}

	Surface::~Surface()
	{
		vkb::destroy_surface(p_window->p_instance->handle, handle);
	}
}
