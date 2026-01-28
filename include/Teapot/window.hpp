													
#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{
	class Surface
	{
		friend class Window;
		friend struct Instance;

	public:
		~Surface();

		Surface(const Surface&) = delete;
		Surface& operator=(const Surface&) = delete;

		vk::SurfaceKHR handle;

		Window* p_window = nullptr;

	private:
		Surface(Window* p_window);
	};

	class Window
	{
		friend class InputManager;
		friend struct Instance;
		friend class Surface;

	private:
		Window(const char* title, glm::uvec2 size, Instance* p_instance);

		std::unique_ptr<Surface> surface;
		std::unique_ptr<InputManager> input;
		vec<std::unique_ptr<Button>> buttons;

	public:
		glm::uvec2 size = { 0, 0 };
		GLFWwindow* handle = nullptr;
		Instance* p_instance;
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = default;
		Window& operator=(Window&&) = default;

		glm::uvec2 getSize() const { return size; }

		Button* addButton(const glm::vec2 bottom_left, const glm::vec2 top_right, ButtonTextures textures, std::function<void()> callback);
		Surface* addSurface();
		InputManager* plugInput();

		void updateButtons() const;
		void updateInput() const;

		const vec<std::unique_ptr<Button>>& getButtons() const { return buttons; }
	};
}

