#include "realsim/core/App.h"
#include "realsim/utils/Random.h"

namespace RSim::Core
{
	auto App::UpdateStatistics::GetFPS() const -> float
	{
		return 1.0f / ((float)m_FrameTime / (float)SDL_GetPerformanceFrequency());
	}

	auto App::UpdateStatistics::GetFrameTime() const -> float
	{
		return (float)m_FrameTime / (float)SDL_GetPerformanceFrequency() * 1000.0f;
	}

	auto App::UpdateStatistics::GetFrameTimeSeconds() const -> float
	{
		return (float)m_FrameTime / (float)SDL_GetPerformanceCounter();
	}

	App::App()
	{
		m_LayerStack.AddLayer(new UI::EditorLayer());
	}

	ReturnCode App::Run()
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


		m_Scene = std::make_unique<ECS::Scene>();
		m_Camera = m_Scene->CreateEntity();
		m_Camera.SetName("Scene Primary Camera");
		pCamera = &m_Camera.AddComponent<ECS::PerspectiveCameraComponent>(true);
		pCamera->NearZ = 0.01f;
        pCamera->FOVHalfAngle = 60.0f;
		pCamera->SetViewMatrix(DirectX::XMMatrixLookAtLH(
			DirectX::XMVectorSet(-5.0f, 10.0f, -10.0f, 1.0f),
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));

        //SusanneAsset = AssetLib::LoadBinaryFile("mesh_assets/monkey0.rsim");

        SusanneAsset = m_AssetCache.Load("mesh_assets/monkey0.rsim");
        SusanneMeshInfo = ReadMeshInfo(*SusanneAsset);

        float shininess = 1.0f;
        for (size_t i = 0; i < 8; ++i)
        {
            for (size_t j = 0; j < 8; ++j)
            {
                m_Monkeys[i][j] = m_Scene->CreateEntity();
                if(j != 0)
                {
                    m_Monkeys[i][0].AddChild(m_Monkeys[i][j]);
                }
                auto& mesh = m_Monkeys[i][j].AddComponent<ECS::MeshComponent>();
                mesh.Drawable = m_Renderer->CreateDrawable(&SusanneMeshInfo, SusanneAsset.get());
                mesh.hAsset = SusanneAsset->HashValue;
                mesh.pMeshInfo = SusanneMeshInfo;
                auto& tc = m_Monkeys[i][j].GetComponent<ECS::TransformComponent>();
                auto& NameComponent = m_Monkeys[i][j].GetComponent<ECS::NameComponent>();
                NameComponent.Name = fmt::format("Susanne {}-{}",i,j);
                float r = std::cosf(Util::Random::RandomFloat()) / 2.5f + 0.5f;
                float g = std::cosf(Util::Random::RandomFloat()) / 2.5f + 0.5f;
                float b = std::cosf(Util::Random::RandomFloat()) / 2.5f + 0.5f;
                mesh.Material.Color = { r, g, b, 1.0f};
                mesh.Material.Shininess = shininess;
                shininess += 2.0f;

                tc.Translation = { static_cast<float>(((float)i - 2.0f) * 2.5f) , 0.0f , static_cast<float>(((float)j - 2.0f) * 2.5f) };
            }
        }

        ECS::Entity parent = m_Monkeys[0][0];
        ECS::Entity child = parent.GetFirstChild();

        for(auto it = child.begin(); it != ECS::Entity::end(); ++it)
        {
            rsim_trace(it->GetName());
        }

#if 0
		m_Box = m_Scene->CreateEntity();
        m_Box.SetName("Susanne 1");
		m_Box2 = m_Scene->CreateEntity();
        m_Box2.SetName("Susanne 2");
        m_Box3 = m_Scene->CreateEntity();
        m_Box3.SetName("Susanne 3");

		auto& Mesh = m_Box.AddComponent<ECS::MeshComponent>();
		auto& Mesh2 = m_Box2.AddComponent<ECS::MeshComponent>();
        auto& Mesh3 = m_Box3.AddComponent<ECS::MeshComponent>();

		Mesh.Drawable = m_Renderer->CreateDrawable(&SusanneMeshInfo, &SusanneAsset);
		Mesh.pAsset = &SusanneAsset;
		Mesh.pMeshInfo = &SusanneMeshInfo;
        Mesh.Material.Color = {0.15f,1.0f,0.3f,1.0f};

		Mesh2.Drawable = Mesh.Drawable;
		Mesh2.pAsset = &SusanneAsset;
		Mesh2.pMeshInfo = &SusanneMeshInfo;
        Mesh2.Material.Color = {1.0f,0.54f,0.3f,1.0f};

        Mesh3.Drawable = Mesh.Drawable;
        Mesh3.pAsset = &SusanneAsset;
        Mesh3.pMeshInfo = &SusanneMeshInfo;
        Mesh3.Material.Shininess = 8.0f;

        m_Box2.GetComponent<ECS::TransformComponent>().Translation.x += 3.0f;
        m_Box3.GetComponent<ECS::TransformComponent>().Translation.z += 3.0f;

        m_Box.AddChild(m_Box2);
        m_Box2.AddChild(m_Box3);
#endif
		m_CameraController.SetEnabled(true);

        SDL_Event e;
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
				m_LayerStack.OnUpdate();
			}

			// Clear the offscreen back buffer and depth buffer
			float color[] = { 0.15f, 0.15f, 0.15f, 1.0f };
			m_Renderer->Clear(color);

			// Render the scene to the offscreen buffer
			m_Renderer->RenderScene(m_Scene.get());

			// UI
			{
				m_Renderer->BeginImGuiFrame();
				m_LayerStack.OnRenderUI();
				m_Renderer->FinishImGuiFrame();
			}

			// Present
			m_Renderer->Present();
			CalculateUpdateStatistics();
		}
		m_LayerStack.OnShutdown();
		return REALSIM_EXIT_SUCCESS;
	}

	void App::Shutdown()
	{
		m_Running = false;
	}

	void App::SetMainWindow(std::unique_ptr<Window> window)
	{
		m_MainWindow = std::move(window);
	}

	void App::AddWindow(std::unique_ptr<Window> window)
	{
		m_OtherWindows.emplace_back(std::move(window));
	}

	ReturnCode App::OnInit()
	{
		m_Renderer = std::make_unique<Graphics::Renderer>(m_MainWindow.get());
		m_MainWindow->SetTitle(fmt::format("RealSim Interactive - {}", m_Renderer->GetEngineName()));
		return REALSIM_EXIT_SUCCESS;
	}

	void App::OnUpdate()
	{
		if (Input::IsKeyPressed(SDL_SCANCODE_ESCAPE))
		{
			Shutdown();
		}

        // Camera controller enable/disable based on input
		{
			// Skip the first few frames because delta time is extremely big and causes issues
			if (m_UpdateStats.m_UpdateCounter < 5)
				return;

			auto& CC = m_Camera.GetComponent<ECS::PerspectiveCameraComponent>();
			if (Input::IsKeyPressed(SDL_SCANCODE_LSHIFT) && Input::IsKeyPressed(SDL_SCANCODE_F1))
				m_CameraController.SetEnabled(false);
			if (Input::IsKeyPressed(SDL_SCANCODE_RETURN))
				m_CameraController.SetEnabled(true);
			m_CameraController.Update(m_UpdateStats.GetFrameTimeSeconds(), &CC);
		}
	}

	void App::Handle_SDL_Events(SDL_Event const& e)
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

	void App::CalculateUpdateStatistics()
	{
		auto const counter = SDL_GetPerformanceCounter();
		m_UpdateStats.m_FrameTime = counter - m_UpdateStats.m_LastTickCount;
		m_UpdateStats.m_LastTickCount = counter;
		++m_UpdateStats.m_UpdateCounter;
	}

	void App::LogLibraryVersion()
	{
		rsim_info("Library Version {}.{}.{}",
			RSIM_PROJECT_VERSION_MAJOR,
			RSIM_PROJECT_VERSION_MINOR,
			RSIM_PROJECT_VERSION_PATCH);
	}

    void App::SetRealSimConsoleTitle(LPCTSTR Title)
	{
		::SetConsoleTitle(Title);
	}
}
