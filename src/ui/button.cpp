#include "Teapot/ui/button.hpp"
#include "Teapot/platform/input.hpp"
#include "Teapot/core/image.hpp"

namespace Teapot
{
	Button::Button(
		const vec2& bottom_left,
		const vec2& top_right,
		ButtonTextures textures,
		std::function<void()> callback
	)
		: bottom_left(bottom_left)
		, top_right(top_right)
		, textures(textures)
		, callback(callback)
	{
	}

	void Button::update(const MouseState& mouse)
	{
		// Disabled buttons don't respond to input
		if (state == ButtonState::Disabled) return;

		bool currently_hovered = contains(static_cast<float>(mouse.x), static_cast<float>(mouse.y));

		if (currently_hovered)
		{
			if (mouse.left_button)
			{
				state = ButtonState::Pressed;
			}
			else
			{
				state = ButtonState::Hovered;

				// Trigger callback on release inside button
				if (was_hovered_last_frame && mouse.left_button_just_released)
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
			state = ButtonState::Normal;
		}

		was_hovered_last_frame = currently_hovered;
	}

	bool Button::contains(float x, float y) const
	{
		return x >= bottom_left.x && x <= top_right.x &&
		       y >= bottom_left.y && y <= top_right.y;
	}

	Texture* Button::getCurrentTexture() const
	{
		switch (state)
		{
			case ButtonState::Hovered:
				return textures.hovered ? textures.hovered : textures.normal;
			case ButtonState::Pressed:
				return textures.pressed ? textures.pressed : textures.normal;
			case ButtonState::Disabled:
				return textures.disabled ? textures.disabled : textures.normal;
			case ButtonState::Normal:
			default:
				return textures.normal;
		}
	}
}

