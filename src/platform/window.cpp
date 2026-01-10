
#include "Teapot/platform/window.hpp"
#include "Teapot/platform/input.hpp"
#include "Teapot/ui/button.hpp"
#include "Teapot/core/instance.hpp"

#include <stdexcept>

namespace Teapot
{
	// --- Window ---
	Window::Window(const char* title, unsigned int w, unsigned int h)
		: width(w), height(h)
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

	void Window::initInput()
	{
		input = std::make_unique<InputManager>(*this);
	}

	Button* Window::newButton(
		const vec2& bottom_left,
		const vec2& top_right,
		ButtonTextures textures,
		std::function<void()> callback
	)
	{
		auto button = std::make_unique<Button>(bottom_left, top_right, textures, callback);
		Button* ptr = button.get();
		buttons.push_back(std::move(button));
		return ptr;
	}

	void Window::updateButtons()
	{
		if (!input) return;

		const auto& mouse = input->getMouse();
		for (auto& button : buttons)
		{
			button->update(mouse);
		}
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
