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

	ReturnCode Application::Run()
	{
		Logger::Init();
		Application::LogLibraryVersion();
		Application::RSIM_SetConsoleTitle("RealSim Interactive - Console");

		if (ReturnCode init = OnInit(); init != REALSIM_EXIT_SUCCESS)
		{
			rsim_error("Initialization has failed!");
			return init;
		}
		SDL_Event e;

		m_Scene = std::make_unique<ECS::Scene>();
		m_Camera = m_Scene->CreateEntity();
		m_Camera.SetName("Scene Primary Camera");
		pCamera = &m_Camera.AddComponent<ECS::PerspectiveCameraComponent>(true);
		pCamera->NearZ = 0.01f;
		pCamera->SetViewMatrix(DirectX::XMMatrixLookAtLH(
			DirectX::XMVectorSet(0.0f, 0.0f, ZPosition, 1.0f),
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));

		m_Box = m_Scene->CreateEntity();
		m_Box2 = m_Scene->CreateEntity();
		auto& BC = m_Box.AddComponent<ECS::BoxComponent>();
		BC.Color = { 0.6f,0.2f,0.2f,1.0f };
		m_Box.GetLocalTransform().Translation.x -= 1.5f;

		auto& BC2 = m_Box2.AddComponent<ECS::BoxComponent>();
		BC2.Color = { 0.6f,0.2f,0.2f,1.0f };
		m_Box2.GetLocalTransform().Translation.x += 1.5f;

		while (m_Running)
		{
			this->CalculateUpdateStatistics();

			m_MainWindow->SetTitle(fmt::format("RealSim Interactive - FPS:{0:.2f}",m_UpdateStats.GetFramesPerSecond()));

			while(SDL_PollEvent(&e) != 0)
			{
				this->Handle_SDL_Events(e);
			}

			this->OnUpdate();

			FLOAT const color[] = { 0.2f / 2.0f,0.2f / 2.0f,0.2f / 2.0f,1.0f};
			m_Renderer->Clear(color);
			m_Renderer->Render(*m_Scene);
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

	ReturnCode Application::OnInit()
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
		if (Input::IsKeyPressed(SDL_SCANCODE_UP))
		{
			ZPosition += 0.01f * m_UpdateStats.GetFrameTime();
			pCamera->SetViewMatrix(DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0.0f, 0.0f, ZPosition, 1.0f),
				DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
				DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
		}
		else if (Input::IsKeyPressed(SDL_SCANCODE_DOWN))
		{
			ZPosition -= 0.01f * m_UpdateStats.GetFrameTime();
			pCamera->SetViewMatrix(DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0.0f, 0.0f, ZPosition, 1.0f),
				DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
				DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
		}

		auto& BC = m_Box.GetComponent<ECS::BoxComponent>();
		auto& BC2 = m_Box2.GetComponent<ECS::BoxComponent>();
		
		BC.Color.x = (float)Input::GetCursorPosition()[0] / (float)m_MainWindow->GetWidth();
		BC.Color.y = (float)Input::GetCursorPosition()[1] / (float)m_MainWindow->GetHeight();
		BC.Color.z = 1.0f - BC.Color.x;

		BC2.Color.x = 1.0f - BC.Color.x;
		BC2.Color.y = 1.0f - BC.Color.y;
		BC2.Color.z = 1.0f - BC.Color.z;
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
					uint32_t height = (uint32_t)e.window.data2;
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

	void Application::RSIM_SetConsoleTitle(LPCTSTR Title)
	{
		::SetConsoleTitle(Title);
	}
}


