#include "realsim/core/Input.h"

namespace RSim::Core
{
	bool Input::IsKeyPressed(SDL_Scancode key)
	{
		const Uint8* state = SDL_GetKeyboardState(NULL);
		return state[key];
	}

	bool Input::IsLeftMouseButtonPressed()
	{
		int64_t buttons = SDL_GetMouseState(NULL, NULL);
		return (buttons & SDL_BUTTON_LMASK) != 0;
	}

	bool Input::IsRightMouseButtonPressed()
	{
		int64_t buttons = SDL_GetMouseState(NULL, NULL);
		return (buttons & SDL_BUTTON_RMASK) != 0;
	}

	Eigen::Vector2i Input::GetCursorPosition()
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		return Eigen::Vector2i{ x, y };
	}
}