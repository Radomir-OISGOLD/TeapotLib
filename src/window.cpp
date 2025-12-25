
#include "Teapot/window.hpp"

#include "Teapot/instance.hpp"

namespace Teapot
{



	Teapot::Window::Window(const char* title, u32t w, u32t h)
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		GLFWwindow* window = glfwCreateWindow(w, h, title, NULL, NULL);

	}

	Window::~Window()
	{
		glfwDestroyWindow(handle);
	}

	Surface::Surface()
	{
		VkResult glfw_result = glfwCreateWindowSurface(p_window->p_instance->handle, p_window->handle, nullptr, &handle);
		if (glfw_result != VK_SUCCESS) {
			std::cerr << "Failed to select create window surface. Error: " << std::to_string(glfw_result) << "\n";
			delete this;
		}
	}

	Surface::~Surface()
	{
		vkb::destroy_surface(p_window->p_instance->handle, handle);
	}

}
