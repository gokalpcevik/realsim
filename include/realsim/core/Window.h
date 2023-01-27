#pragma once
#include <cstdint>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <cassert>

#define RSIM_ENABLE_ASSERTS

#include "Assert.h"

namespace RSim::Core
{
	struct WindowDescriptor
	{
		/**
		 * \brief Title of the window.
		 */
		std::string Title;
		/**
		 * \brief Width
		 */
		std::int32_t Width;
		/**
		 * \brief Height
		 */
		std::int32_t Height;

		/**
		 * \brief X position of the window in the screen.
		 */
		std::int32_t X;
		/**
		 * \brief Y position of the window in the screen.
		 */
		std::int32_t Y;
		/**
		 * \brief Window flags passed to the SDL_CreateWindow() function.
		 */
		Uint32 Flags;
	};

	class Window
	{
	public:
		Window(WindowDescriptor const& desc);

		Window(Window const&) = delete;
		Window& operator=(Window const&) = delete;
		virtual ~Window();

		virtual void HandleEvent(SDL_Event const& e);

		std::uint32_t GetWidth() const;
		std::uint32_t GetHeight() const;

		[[nodiscard]] SDL_Window* GetSDLWindow() const;
		[[nodiscard]] HWND GetHWND() const;
 	private:
		/**
		 * \brief Member variable(pointer) to the SDL_Window structure, allocated in the constructor with
		 * the SDL_CreateWindow() call.
		 */
		SDL_Window* m_SDL_Window = nullptr;
	};
}