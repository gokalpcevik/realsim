#pragma once
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mouse.h>
#include <Eigen/Eigen>

namespace RSim::Core
{

	/**
	 * \brief A simple input class making use of SDL's functionality.
	 */
	class Input final
	{
	public:
		Input() = default;
		Input(const Input&) = delete;
		Input& operator=(Input const&) = delete;

		[[nodiscard]] static bool IsKeyPressed(SDL_Scancode key);
		[[nodiscard]] static bool IsLeftMouseButtonPressed();
		[[nodiscard]] static bool IsRightMouseButtonPressed();
		[[nodiscard]] static Eigen::Vector2i GetCursorPosition();
	};
}