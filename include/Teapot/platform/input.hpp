
#pragma once

#include "Teapot/common/cap.hpp"
#include "glm/vec2.hpp"

namespace Teapot
{
	struct MouseState
	{
		glm::vec2 loc = { 0, 0 };
		glm::vec2 prev_loc = { 0, 0 };

		bool lmb =		false;
		bool prev_lmb = false;
		bool rmb =		false;
		bool prev_rmb = false;
		bool mmb =		false;
		bool prev_mmb =	false;

		double scroll_offset = 0.0;
		double prev_offset = 0.0;
	};

	// TODO - Add keyboard
	class InputManager
	{
		friend class Window;

		Window* p_window = nullptr;
		MouseState mouse;
	public:
		InputManager(Window* p_window);
		~InputManager();

		// Non-copyable and non-movable
		InputManager(const InputManager&) = delete;
		InputManager& operator=(const InputManager&) = delete;
		InputManager(InputManager&&) = delete;
		InputManager& operator=(InputManager&&) = delete;

		// Update input state - call once per frame
		void update();

		const MouseState& getMouse() const { return mouse; }

		// GLFW callbacks (must be static)
		static void clickCallback_GLFW(GLFWwindow* window, int button, int action, int mods);
		static void cursorCallback_GLFW(GLFWwindow* window, double x_pos, double y_pos);
		static void scrollCallback_GLFW(GLFWwindow* window, double x_offset, double y_offset);
	};
}

