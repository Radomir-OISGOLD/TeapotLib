
#include "Teapot/platform/window.hpp"
#include "Teapot/platform/input.hpp"
#include "Teapot/ui/button.hpp"
#include "Teapot/core/instance.hpp"
#include "Teapot/common/structures.hpp"

#include <stdexcept>

namespace Teapot
{
	Window::Window(const char* title, unsigned int w, unsigned int h, Init& init)
		: width(w), height(h)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		handle = glfwCreateWindow(w, h, title, NULL, NULL);
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
		auto button = std::make_unique<Surface>();
		Button* ptr = button.get();
		buttons.push_back(std::move(button));
		return ptr;
	}

	InputManager* Window::plugInput()
	{
		auto button = std::make_unique<Button>(bottom_left, top_right, textures, callback);
		Button* ptr = button.get();
		buttons.push_back(std::move(button));
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
		VkResult result = glfwCreateWindowSurface(init.p_instance->handle, init.p_window->handle, nullptr, &handle);
		if (result != VK_SUCCESS) {
			err("Failed to create window surface. Error: " + std::to_string(result));
		}
	}

	Surface::~Surface()
	{
		printf("aaa");
		vkb::destroy_surface(p_instance->handle, handle);
		printf("bbb");
	}
}
