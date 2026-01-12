#include "Teapot/platform/input.hpp"
#include "Teapot/platform/window.hpp"
#include "Teapot/common/structures.hpp"

namespace Teapot
{
	InputManager::InputManager(Init* init)
		: p_window(init->p_window)
	{
		// Set this InputManager as the user pointer for GLFW callbacks
		glfwSetWindowUserPointer(p_window->handle, this);

		// Set up GLFW callbacks
		glfwSetMouseButtonCallback(p_window->handle, mouseButtonCallback);
		glfwSetCursorPosCallback(p_window->handle, cursorPosCallback);
	}

	InputManager::~InputManager()
	{
		if (p_window && p_window->handle)
		{
			glfwSetMouseButtonCallback(p_window->handle, nullptr);
			glfwSetCursorPosCallback(p_window->handle, nullptr);
			glfwSetWindowUserPointer(p_window->handle, nullptr);
		}
	}

	void InputManager::update()
	{
		// Update previous state
		mouse_state.prev_x = mouse_state.x;
		mouse_state.prev_y = mouse_state.y;
		mouse_state.left_button_prev = mouse_state.left_button;

		// Compute just pressed/released states
		mouse_state.left_button_just_pressed = mouse_state.left_button && !mouse_state.left_button_prev;
		mouse_state.left_button_just_released = !mouse_state.left_button && mouse_state.left_button_prev;
	}

	void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		InputManager* input_manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		if (!input_manager) return;

		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			input_manager->mouse_state.left_button = (action == GLFW_PRESS);
		}
	}

	void InputManager::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
		InputManager* input_manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		if (!input_manager) return;

		input_manager->mouse_state.x = xpos;
		input_manager->mouse_state.y = ypos;
	}
}

