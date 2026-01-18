
#pragma once

#include <functional>

#include "deps/glm/vec2.hpp"
#include "Teapot/common/cap.hpp"


namespace Teapot
{

	enum class ButtonState
	{
		IDLE,
		HOVER,
		PRESS,
		OFF
	};

	struct ButtonTextures
	{
		Texture* idle = nullptr;
		Texture* hover = nullptr;
		Texture* press = nullptr;
		Texture* off = nullptr;
	};

	class Button
	{
	public:

		Button(const glm::vec2 bottom_left, const glm::vec2 top_right, 
			ButtonTextures textures, std::function<void()> callback);

		void update(const MouseState& mouse);

		// Check if point is inside button bounds
		bool is_hovered(glm::vec2 loc) const;

		// Get current texture based on state
		Texture* getCurrentTexture() const;

		// Getters
		const glm::vec2 getBottomLeft() const { return bottom_left; }
		const glm::vec2 getTopRight() const { return top_right; }
		ButtonState getState() const { return state; }

		void setState(ButtonState state)
		{
			this->state = state;
		}

	private:
		glm::vec2 bottom_left;
		glm::vec2 top_right;

		ButtonTextures textures;
		ButtonState state = ButtonState::IDLE;
		bool was_hovered_last_frame = false;

		std::function<void()> callback;
		
	};
}

