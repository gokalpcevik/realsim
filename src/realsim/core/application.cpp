#include "realsim/core/Application.h"


namespace rsim::core
{
	auto Application::UpdateStatistics::GetFramesPerSecond() const -> float
	{
		return 1.0f / ((float)m_FrameTime / (float)SDL_GetPerformanceFrequency());
	}

	auto Application::UpdateStatistics::GetFrameTime() const -> float
	{
		return (float)m_FrameTime / (float)SDL_GetPerformanceFrequency() * 1000.0f;
	}

	auto Application::UpdateStatistics::GetFrameTimeSeconds() const -> float
	{
		return (float)m_FrameTime / (float)SDL_GetPerformanceCounter();
	}

	Application::Application(CommandLineArgs args)
    : m_StartArgs(args)
	{

	}

	return_code Application::Run()
	{
		Logger::Init();

		auto device = std::make_unique<graphics::GraphicsDevice>();
		auto swapchain = std::make_unique<graphics::SwapChain>(device->GetFactory3Raw(), m_MainWindow.get(), *device);

		SDL_Event e;

		while (m_Running)
		{
			this->CalculateUpdateStatistics();

			while(SDL_PollEvent(&e) != 0)
			{
				this->HandleSDLEvents(e);
			}

			this->OnUpdate();


		}
		return REALSIM_EXIT_SUCCESS;
	}

	void Application::Shutdown()
	{
		m_Running = false;
	}

	void Application::SetMainWindow(std::unique_ptr<Window> window)
	{
		m_MainWindow = std::move(window);
	}

	void Application::AddWindow(std::unique_ptr<Window> window)
	{
		m_OtherWindows.emplace_back(std::move(window));
	}

	void Application::OnUpdate()
	{
	}

	void Application::HandleSDLEvents(SDL_Event const& e)
	{
		if (e.type == SDL_QUIT)
		{
			m_Running = false;
		}
		// Handle window events with e.window.event if the e.type == SDL_WINDOWEVENT
		if (e.type == SDL_WINDOWEVENT)
		{
			if (e.window.windowID == SDL_GetWindowID(m_MainWindow->GetSDLWindow()))
			{
				m_MainWindow->HandleEvent(e);
			}
			else
			{
				for (auto const& window : m_OtherWindows)
				{
					window->HandleEvent(e);
				}
			}
		}
	}

	void Application::CalculateUpdateStatistics()
	{
		auto const counter = SDL_GetPerformanceCounter();
		m_UpdateStats.m_FrameTime = counter - m_UpdateStats.m_LastTickCount;
		m_UpdateStats.m_LastTickCount = counter;
	}
}


