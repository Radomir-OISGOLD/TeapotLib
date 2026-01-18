#include "Teapot/platform/input.hpp"
#include "Teapot/platform/window.hpp"
#include "Teapot/common/structures.hpp"

namespace Teapot
{
	InputManager::InputManager(Window& window)
	{
		// Set this InputManager as the user pointer for GLFW callbacks.
		glfwSetWindowUserPointer(window.handle, this);

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

	void InputManager::cursorCallback_GLFW(GLFWwindow* window, double xpos, double ypos)
	{
		InputManager* p_input_manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		if (!p_input_manager) return;

		p_input_manager->mouse.loc = { xpos, ypos };
	}

	void InputManager::scrollCallback_GLFW(GLFWwindow* window, double x_offset, double y_offset)
	{
		InputManager* p_input_manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		if (!p_input_manager) return;

		p_input_manager->mouse.scroll_offset = y_offset;
	}
}

