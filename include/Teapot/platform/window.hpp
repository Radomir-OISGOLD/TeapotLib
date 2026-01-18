													
#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	/*
	 * Requires Window and Instance objects to be initialized.
	 */
	class Surface
	{
		friend class Window;

		Surface(Window* p_window);
		~Surface();

		Surface(const Surface&) = delete;
		Surface& operator=(const Surface&) = delete;
		Surface(Surface&&) = default;
		Surface& operator=(Surface&&) = default;

		VkSurfaceKHR handle = VK_NULL_HANDLE;

		Window* p_window = nullptr;
	};

	class Window
	{
		friend class Surface;
		friend class InputManager;

		glm::vec2<unsigned int> size = { 0, 0 };

		GLFWwindow* handle = nullptr;
		Instance* p_instance;

		std::unique_ptr<Surface> surface;
		std::unique_ptr<InputManager> input;
		vec<std::unique_ptr<Button>> buttons;

	public:
		Window(const char* title, glm::vec2<unsigned int> size, Instance& instance);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = default;
		Window& operator=(Window&&) = default;

		glm::vec2<unsigned int> getSize() const { return size; }

		Button* addButton(const glm::vec2 bottom_left, const glm::vec2 top_right,
			ButtonTextures textures, std::function<void()> callback);
		Surface* addSurface();
		InputManager* plugInput();

		void updateButtons() const;

		const vec<std::unique_ptr<Button>>& getButtons() const { return buttons; }
	};
}

