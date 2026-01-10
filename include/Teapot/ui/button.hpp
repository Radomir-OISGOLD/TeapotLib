
#pragma once

#include "Teapot/common/cap.hpp"
#include <functional>

namespace Teapot
{
	// Simple 2D vector for pixel coordinates
	struct vec2
	{
		float x, y;
		vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
	};

	// Button states for visual feedback
	enum class ButtonState
	{
		Normal,
		Hovered,
		Pressed,
		Disabled
	};

	// Texture pointers for each button state
	struct ButtonTextures
	{
		Texture* normal = nullptr;
		Texture* hovered = nullptr;
		Texture* pressed = nullptr;
		Texture* disabled = nullptr;
	};

	class Button
	{
	public:
		// Constructor matching user requirements
		Button(
			const vec2& bottom_left,
			const vec2& top_right,
			ButtonTextures textures,
			std::function<void()> callback
		);

		// Update button state based on mouse input
		void update(const MouseState& mouse);

		// Check if point is inside button bounds
		bool contains(float x, float y) const;

		// Get current texture based on state
		Texture* getCurrentTexture() const;

		// Getters
		const vec2& getBottomLeft() const { return bottom_left; }
		const vec2& getTopRight() const { return top_right; }
		ButtonState getState() const { return state; }

		void setEnabled(bool enabled)
		{
			state = enabled ? ButtonState::Normal : ButtonState::Disabled;
		}

	private:
		vec2 bottom_left;
		vec2 top_right;
		ButtonTextures textures;
		std::function<void()> callback;
		ButtonState state = ButtonState::Normal;
		bool was_hovered_last_frame = false;
	};
}

