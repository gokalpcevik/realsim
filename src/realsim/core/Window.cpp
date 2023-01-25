#include "realsim/core/Window.h"

namespace rsim::core
{
	Window::Window(WindowDescriptor const& desc)
	{
		m_SDL_Window = SDL_CreateWindow(desc.Title.c_str(), desc.X, desc.Y, desc.Width, desc.Height, desc.Flags);
		RSIM_ASSERTM(m_SDL_Window != nullptr, "SDL_CreateWindow() has returned an invalid window.");
	}

	Window::~Window()
	{
		SDL_DestroyWindow(m_SDL_Window);
	}

	void Window::HandleEvent(SDL_Event const& e)
	{
		switch(e.window.event)
		{
			case SDL_WINDOWEVENT_CLOSE:
				break;
			// Handle other window events when we implement a renderer
		}
	}

	std::uint32_t Window::GetWidth() const
	{
		int w = 0;
		SDL_GetWindowSize(m_SDL_Window, &w, nullptr);
		return w;
	}

	std::uint32_t Window::GetHeight() const
	{
		int h = 0;
		SDL_GetWindowSize(m_SDL_Window, nullptr, &h);
		return h;
	}

	SDL_Window* Window::GetSDLWindow() const
	{
		return m_SDL_Window;
	}

	HWND Window::GetHWND() const
	{
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(m_SDL_Window, &info);
		HWND hwnd = info.info.win.window;
		return hwnd;
	}
}