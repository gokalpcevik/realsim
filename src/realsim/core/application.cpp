#include "realsim/core/Application.h"
#include <EngineFactory.h>
#include <EngineFactoryD3D12.h>

namespace RSim::Core
{
	auto Application::UpdateStatistics::GetFPS() const -> float
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
		: m_StartArgs(args), m_LayerStack(*this)
	{
		m_UILayer = new Editor::UILayer();
		m_LayerStack.AddLayer(m_UILayer);
	}

	ReturnCode Application::Run()
	{
		Logger::Init();
		LogLibraryVersion();
		SetRealSimConsoleTitle(L"RealSim Interactive - Console");

		if (ReturnCode init = OnInit(); init != REALSIM_EXIT_SUCCESS)
		{
			rsim_error("Initialization has failed!");
			return init;
		}
		m_LayerStack.OnInit();

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
		auto& Mesh = m_Box.AddComponent<ECS::MeshComponent>();
		auto& Mesh2 = m_Box2.AddComponent<ECS::MeshComponent>();

		SusanneAsset = AssetLib::LoadBinaryFile("mesh_assets/monkey0.rsim");
		SusanneMeshInfo = ReadMeshInfo(SusanneAsset);
		Mesh.Drawable = m_Renderer->CreateDrawable(&SusanneMeshInfo, &SusanneAsset);
		Mesh.pAsset = &SusanneAsset;
		Mesh.pMeshInfo = &SusanneMeshInfo;

		Mesh2.Drawable = Mesh.Drawable;
		Mesh2.pAsset = &SusanneAsset;
		Mesh2.pMeshInfo = &SusanneMeshInfo;

		m_Box2.GetComponent<ECS::TransformComponent>().Translation.x += 3.0f;

		m_CameraController.SetEnabled(true);

		while (m_Running)
		{

			while (SDL_PollEvent(&e) != 0)
			{
				Handle_SDL_Events(e);
				ImGui_ImplSDL2_ProcessEvent(&e);
			}

			// Update
			{
				this->OnUpdate();
				m_LayerStack.OnUpdate(m_Scene.get());
			}

			// Clear the offscreen back buffer and depth buffer
			float color[] = { 0.15f, 0.15f, 0.15f, 1.0f };
			m_Renderer->Clear(color);

			// Render the scene to the offscreen buffer
			m_Renderer->RenderScene(m_Scene.get());

			// UI
			{
				m_Renderer->BeginImGuiFrame();
				m_LayerStack.OnRenderUI(m_Scene.get(), m_Renderer.get());
				m_Renderer->FinishImGuiFrame();
			}

			// Present
			m_Renderer->Present(1u);
			CalculateUpdateStatistics();
		}
		m_LayerStack.OnShutdown();
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
		m_Renderer = std::make_unique<Graphics::Renderer>(m_MainWindow.get());
		m_MainWindow->SetTitle(fmt::format("RealSim Interactive - {}", m_Renderer->GetEngineName()));
		return REALSIM_EXIT_SUCCESS;
	}

	void Application::OnUpdate()
	{
		if (Input::IsKeyPressed(SDL_SCANCODE_ESCAPE))
		{
			Shutdown();
		}

		{
			// Skip the first few frames because delta time is extremely big and causes issues
			if (m_UpdateStats.m_UpdateCounter < 5)
				return;

			auto& CC = m_Camera.GetComponent<ECS::PerspectiveCameraComponent>();
			if (Input::IsKeyPressed(SDL_SCANCODE_LSHIFT) && Input::IsKeyPressed(SDL_SCANCODE_F1))
				m_CameraController.SetEnabled(false);
			if (Input::IsKeyPressed(SDL_SCANCODE_KP_ENTER))
				m_CameraController.SetEnabled(true);
			m_CameraController.Update(m_UpdateStats.GetFrameTimeSeconds(), &CC);
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
				m_MainWindow->HandleEvent(e, true);
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					uint32_t width = (uint32_t)e.window.data1;
					uint32_t height = (uint32_t)e.window.data2;
					m_Renderer->Resize(width, height);
				}
			}
			else
			{
				for (auto const& window : m_OtherWindows)
				{
					window->HandleEvent(e, false);
				}
			}
		}
	}

	void Application::CalculateUpdateStatistics()
	{
		auto const counter = SDL_GetPerformanceCounter();
		m_UpdateStats.m_FrameTime = counter - m_UpdateStats.m_LastTickCount;
		m_UpdateStats.m_LastTickCount = counter;
		++m_UpdateStats.m_UpdateCounter;
	}

	void Application::LogLibraryVersion()
	{
		rsim_info("Library Version {}.{}.{}",
			RSIM_PROJECT_VERSION_MAJOR,
			RSIM_PROJECT_VERSION_MINOR,
			RSIM_PROJECT_VERSION_PATCH);
	}

	void Application::SetRealSimConsoleTitle(LPCTSTR Title)
	{
		::SetConsoleTitle(Title);
	}
}
