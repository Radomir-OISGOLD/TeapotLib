#include "Teapot/input.hpp"
#include "Teapot/window.hpp"

namespace Teapot
{
	InputManager::InputManager(Window* p_window)
		: p_window(p_window)
	{
		// Set this InputManager as the user pointer for GLFW callbacks.
		glfwSetWindowUserPointer(p_window->handle, this);

		glfwSetMouseButtonCallback(p_window->handle, clickCallback_GLFW);
		glfwSetCursorPosCallback(p_window->handle, cursorCallback_GLFW);
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
		// Update previous states
		mouse.prev_loc = mouse.loc;

		mouse.prev_lmb = mouse.lmb;
		mouse.prev_rmb = mouse.rmb;
		mouse.prev_mmb = mouse.mmb;

		mouse.prev_offset = mouse.scroll_offset;
	}

	void InputManager::clickCallback_GLFW(GLFWwindow* window, int button, int action, int mods)
	{
		InputManager* input_manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		if (!input_manager) return;

		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			input_manager->mouse.lmb = (action == GLFW_PRESS);
		case GLFW_MOUSE_BUTTON_RIGHT:
			input_manager->mouse.rmb = (action == GLFW_PRESS);
		case GLFW_MOUSE_BUTTON_MIDDLE:
			input_manager->mouse.mmb = (action == GLFW_PRESS);
		default:
			return;
		}
	}

	void InputManager::cursorCallback_GLFW(GLFWwindow* window, double x_pos, double y_pos)
	{
		InputManager* p_input_manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		if (!p_input_manager) return;

		p_input_manager->mouse.loc = { x_pos, y_pos };
	}

	void InputManager::scrollCallback_GLFW(GLFWwindow* window, double x_offset, double y_offset)
	{
		InputManager* p_input_manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		if (!p_input_manager) return;

		p_input_manager->mouse.scroll_offset = y_offset;
	}
}

