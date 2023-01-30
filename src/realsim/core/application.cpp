#include "realsim/core/Application.h"


namespace RSim::Core
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
		Application::LogLibraryVersion();

		if(return_code init = OnInit(); init != REALSIM_EXIT_SUCCESS)
		{
			rsim_error("Initialization has failed!");
			return init;
		}

		SDL_Event e;

		while (m_Running)
		{
			this->CalculateUpdateStatistics();

			while(SDL_PollEvent(&e) != 0)
			{
				this->Handle_SDL_Events(e);
			}

			this->OnUpdate();

			FLOAT const color[] = { 0.021f,0.021f,0.171f,1.0f};
			m_Renderer->Clear(color);
			m_Renderer->Present();
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

	return_code Application::OnInit()
	{
		m_Renderer = std::make_unique<GFX::Renderer>(m_MainWindow.get());
		return REALSIM_EXIT_SUCCESS;
	}

	void Application::OnUpdate()
	{
		if(Input::IsKeyPressed(SDL_SCANCODE_ESCAPE))
		{
			Shutdown();
		}
	}

	void Application::Handle_SDL_Events(SDL_Event const& e)
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
				m_MainWindow->HandleEvent(e,true);
				if(e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					uint32_t width  = (uint32_t)e.window.data1;
					uint32_t height = (uint32_t)e.window.data1;
					m_Renderer->Resize(width, height);
				}
			}
			else
			{
				for (auto const& window : m_OtherWindows)
				{
					window->HandleEvent(e,false);
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

	void Application::LogLibraryVersion()
	{
		rsim_info("Library Version {}.{}.{}", 
			RSIM_PROJECT_VERSION_MAJOR,
		    RSIM_PROJECT_VERSION_MINOR, 
			RSIM_PROJECT_VERSION_PATCH);
	}
}


