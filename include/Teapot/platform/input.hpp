
#pragma once

#include "Teapot/common/cap.hpp"

namespace Teapot
{
	struct MouseState
	{
		double x = 0.0;
		double y = 0.0;
		double prev_x = 0.0;
		double prev_y = 0.0;

		bool left_button = false;
		bool left_button_prev = false;
		bool left_button_just_pressed = false;
		bool left_button_just_released = false;
	};

	class InputManager
	{
	public:
		InputManager(Init* init);
		~InputManager();

		// Non-copyable and non-movable
		InputManager(const InputManager&) = delete;
		InputManager& operator=(const InputManager&) = delete;
		InputManager(InputManager&&) = delete;
		InputManager& operator=(InputManager&&) = delete;

		// Update input state - call once per frame
		void update();

		const MouseState& getMouse() const { return mouse_state; }

		// GLFW callback handlers (must be static)
		static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

	private:
		Window* p_window = nullptr;
		MouseState mouse_state;
	};
}

