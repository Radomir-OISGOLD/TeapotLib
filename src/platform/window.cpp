
#include "Teapot/platform/window.hpp"
#include "Teapot/platform/input.hpp"
#include "Teapot/ui/button.hpp"
#include "Teapot/core/instance.hpp"
#include "Teapot/common/structures.hpp"

#include <stdexcept>

namespace Teapot
{
	Window::Window(const char* title, glm::vec2<unsigned int> size, Init& init) :
		size(size)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		handle = glfwCreateWindow(size.x(), size.y(), title, nullptr, nullptr);
		if (!handle) {
			err("Failed to create GLFW window.");
		}
		init.p_window = this;
	}

	Window::~Window()
	{
		if (handle) {
			glfwDestroyWindow(handle);
		}
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
		auto surface = std::make_unique<Surface>(this);
		Surface* ptr = surface.get();
		this->surface = std::move(surface);
		return ptr;
	}

	InputManager* Window::plugInput()
	{
		auto input = std::make_unique<InputManager>(this);
		InputManager* ptr = input.get();
		this->input = std::move(input);
		return ptr;
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
		VkResult result = glfwCreateWindowSurface(p_window->p_instance->handle, p_window->handle, nullptr, &handle);
		if (result != VK_SUCCESS) {
			err("Failed to create window surface. Error: " + std::to_string(result));
		}
	}

	Surface::~Surface()
	{
		printf("aaa");
		vkb::destroy_surface(p_window->p_instance->handle, handle);
		printf("bbb");
	}
}
