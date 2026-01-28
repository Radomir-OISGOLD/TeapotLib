#include "Teapot/button.hpp"
#include "Teapot/input.hpp"
#include "Teapot/image.hpp"

namespace Teapot
{
	Button::Button(const glm::vec2 bottom_left, const glm::vec2 top_right,
		ButtonTextures textures, std::function<void()> callback) :
		bottom_left(bottom_left),
		top_right(top_right),
		textures(textures),
		callback(callback){}

	void Button::update(const MouseState& mouse)
	{
		// Disabled buttons don't respond to input
		if (state == ButtonState::OFF) return;

		bool currently_hovered = is_hovered(mouse.loc);
		bool left_button_just_released = !mouse.lmb && mouse.prev_lmb;

		if (currently_hovered)
		{
			if (mouse.lmb)
			{
				state = ButtonState::PRESS;
			}
			else
			{
				state = ButtonState::HOVER;

				// Trigger callback on release inside button
				if (was_hovered_last_frame && left_button_just_released)
				{
					if (callback)
					{
						callback();
					}
				}
			}
		}
		else
		{
			state = ButtonState::IDLE;
		}

		was_hovered_last_frame = currently_hovered;
	}

	bool Button::is_hovered(glm::vec2 loc) const
	{
		return loc.x >= bottom_left.x && loc.x <= top_right.x &&
			loc.y >= bottom_left.y && loc.y <= top_right.y;
	}

	Texture* Button::getCurrentTexture() const
	{
		switch (state)
		{
			case ButtonState::HOVER:
				return textures.hover ? textures.hover : textures.idle;
			case ButtonState::PRESS:
				return textures.press ? textures.press : textures.idle;
			case ButtonState::OFF:
				return textures.off ? textures.off : textures.idle;
			case ButtonState::IDLE:
			default:
				return textures.idle;
		}
	}
}

