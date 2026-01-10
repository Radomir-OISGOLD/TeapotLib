
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
		void initInput();

		// Button management API
		Button* newButton(
			const vec2& bottom_left,
			const vec2& top_right,
			ButtonTextures textures,
			std::function<void()> callback
		);

		void updateButtons();

		unsigned int getWidth() const { return width; }
		unsigned int getHeight() const { return height; }
		const vec<std::unique_ptr<Button>>& getButtons() const { return buttons; }

		GLFWwindow* handle = nullptr;
		std::unique_ptr<Surface> surface;
		std::unique_ptr<InputManager> input;

	private:
		unsigned int width = 0;
		unsigned int height = 0;
		vec<std::unique_ptr<Button>> buttons;
	};
}

