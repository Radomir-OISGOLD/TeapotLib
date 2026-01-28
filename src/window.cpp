
#include "Teapot/window.hpp"
#include "Teapot/input.hpp"
#include "Teapot/button.hpp"
#include "Teapot/instance.hpp"

#include <stdexcept>

namespace Teapot
{
	Window::Window(const char* title, glm::uvec2 size, Instance* p_instance) :
		size(size), p_instance(p_instance)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		handle = glfwCreateWindow(size.x, size.y, title, nullptr, nullptr);
		if (!handle)
			err("Failed to create GLFW window.");
	}

	Window::~Window()
	{
		if (handle)
			glfwDestroyWindow(handle);
	}

	Button* Window::addButton(const glm::vec2 bottom_left, const glm::vec2 top_right,
		ButtonTextures textures, std::function<void()> callback)
	{
		auto button = std::make_unique<Button>(bottom_left, top_right, textures, callback);
		Button* ptr = button.get();
		buttons.push_back(std::move(button));
		return ptr;
	}

	Surface* Window::addSurface()
	{
		auto surface = std::unique_ptr<Surface>(new Surface(this));
		Surface* ptr = surface.get();
		this->surface = std::move(surface);
		return ptr;
	}

	InputManager* Window::plugInput()
	{
		auto input = std::unique_ptr<InputManager>(new InputManager(this));
		InputManager* ptr = input.get();
		this->input = std::move(input);
		return ptr;
	}


	void Window::updateInput() const
	{
		input->update();
	}

	void Window::updateButtons() const
	{
		if (!input) return;

		const auto& mouse = input->getMouse();
		for (auto& button : buttons)
		{
			button->update(mouse);
		}
	}

	Surface::Surface(Window* p_window) :
		p_window(p_window)
	{
		VkSurfaceKHR c_surface;
		VkResult result = glfwCreateWindowSurface(p_window->p_instance->vkb_instance.instance, p_window->handle, nullptr, &c_surface);
		isVkOk(static_cast<vk::Result>(result), "Failed to create window surface");
		handle = static_cast<vk::SurfaceKHR>(c_surface);
	}

	Surface::~Surface()
	{
		if (handle && p_window && p_window->p_instance)
			p_window->p_instance->instance.destroySurfaceKHR(handle);
	}
}
