#include "realsim/core/Window.h"

namespace RSim::Core
{
	Window::Window(WindowDescription const& desc)
	{
		m_SDL_Window = SDL_CreateWindow(desc.Title.c_str(), desc.X, desc.Y, desc.Width, desc.Height, desc.Flags);
		RSIM_ASSERTM(m_SDL_Window != nullptr, "SDL_CreateWindow() has returned an invalid window.");
	}

	Window::~Window()
	{
		SDL_DestroyWindow(m_SDL_Window);
	}

	void Window::HandleEvent(SDL_Event const& e,bool IsMainWindow)
	{
		switch(e.window.event)
		{
			case SDL_WINDOWEVENT_CLOSE:
				break;
			default: 
				break;
			// Handle other window events when we implement a renderer
		}
	}

	void Window::SetTitle(std::string const& window) const
	{
		SDL_SetWindowTitle(m_SDL_Window, window.c_str());
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

	bool Window::IsMinimized() const
	{
		return SDL_GetWindowFlags(m_SDL_Window) & SDL_WINDOW_MINIMIZED;
	}

	bool Window::IsMaximized() const
	{
		return SDL_GetWindowFlags(m_SDL_Window) & SDL_WINDOW_MAXIMIZED;
	}

	bool Window::HasInputFocus() const
	{
		return SDL_GetWindowFlags(m_SDL_Window) & SDL_WINDOW_INPUT_FOCUS;
	}

	bool Window::HasGrabbedInput() const
	{
		return SDL_GetWindowFlags(m_SDL_Window) & SDL_WINDOW_INPUT_GRABBED;
	}

	bool Window::HasMouseFocus() const
	{
		return SDL_GetWindowFlags(m_SDL_Window) & SDL_WINDOW_MOUSE_FOCUS;
	}

	bool Window::HasGrabbedMouse() const
	{
		return SDL_GetWindowFlags(m_SDL_Window) & SDL_WINDOW_MOUSE_GRABBED;
	}
}
