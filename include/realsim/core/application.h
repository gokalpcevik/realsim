#pragma once
#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>


#include "realsim/RealSim.h"
#include "realsim/core/Window.h"
#include "realsim/core/Input.h"
#include "realsim/core/Logger.h"

#include "realsim/graphics/Renderer.h"
#include "realsim/graphics/RealSimGraphics.h"
#include "realsim/graphics/FreeListAllocator.h"

#include "realsim/ecs/Scene.h"
#include "realsim/ecs/Entity.h"
#include "realsim/ecs/PerspectiveCameraComponent.h"
#include "realsim/ecs/CommonComponents.h"
#include "realsim/ecs/Link.h"

#include "RealSimConfig.h"

#define REALSIM_EXIT_SUCCESS 0
#define REALSIM_EXIT_FAIL (-1)
#define REALSIM_EXIT_CONTINUE (1)

namespace RSim::Core
{
	class Application
	{
	public:
        struct UpdateStatistics
        {
            Uint64 m_LastTickCount = 0ULL;
            Uint64 m_FrameTime = 0ULL;

            [[nodiscard]] auto GetFramesPerSecond() const -> float;

            [[nodiscard]] auto GetFrameTime() const -> float;

            [[nodiscard]] auto GetFrameTimeSeconds() const -> float;
        };

        struct CommandLineArgs
        {
            CommandLineArgs() = default;
            int Count;
            char** Args;
        };

	public:
		explicit Application(CommandLineArgs args);
        virtual ~Application() = default;

		ReturnCode Run();
        virtual void Shutdown();

        void SetMainWindow(std::unique_ptr<Window> window);
        void AddWindow(std::unique_ptr<Window> window);

    protected:
        virtual ReturnCode OnInit();
        virtual void OnUpdate();
    private:
        void Handle_SDL_Events(SDL_Event const& e);
        void CalculateUpdateStatistics();
        static void LogLibraryVersion();
        static void RSIM_SetConsoleTitle(LPCTSTR Title);
	protected:
        CommandLineArgs m_StartArgs{};
        bool m_Running = true;
        UpdateStatistics m_UpdateStats{};

        std::unique_ptr<Window> m_MainWindow;
        std::vector<std::unique_ptr<Window>> m_OtherWindows;

        std::unique_ptr<GFX::Renderer> m_Renderer;

        std::unique_ptr<ECS::Scene> m_Scene;
        ECS::Entity m_Box;
        ECS::Entity m_Camera;
        ECS::PerspectiveCameraComponent* pCamera;

        float ZPosition = -3.0f;
	};
}
