#pragma once
#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>


#include "realsim/RealSim.h"
#include "realsim/core/Window.h"
#include "realsim/core/Input.h"
#include "realsim/core/Logger.h"

#include "realsim/graphics-de/Renderer.h"
#include "realsim/graphics-de/AssetCache.h"

#include "realsim/ecs/Scene.h"
#include "realsim/ecs/Entity.h"
#include "realsim/ecs/CameraComponent.h"
#include "realsim/ecs/CommonComponents.h"
#include "realsim/ecs/Link.h"

#include "assetlib/AssetLoader.h"
#include "assetlib/MeshLoader.h"

#include "realsim/ui/CameraController.h"
#include "realsim/ui/LayerStack.h"
#include "realsim/ui/EditorLayer.h"

#include "RealSimConfig.h"

#define REALSIM_EXIT_SUCCESS EXIT_SUCCESS
#define REALSIM_EXIT_FAIL (EXIT_FAILURE)
#define REALSIM_EXIT_CONTINUE (2)

namespace RSim::Core
{
    class App
    {
    public:
        struct UpdateStatistics
        {
            Uint64 m_LastTickCount = 0ULL;
            Uint64 m_FrameTime = 0ULL;
            uint64_t m_UpdateCounter = 0ULL;

            [[nodiscard]] auto GetFPS() const -> float;

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
        static App& Get()
        {
            static App instance;
            return instance;
        }
        App();
        App(App const&) = delete;
        App operator=(App const&) = delete;

        ReturnCode Run();
        virtual void Shutdown();

        void SetMainWindow(std::unique_ptr<Window> window);
        void AddWindow(std::unique_ptr<Window> window);
        /**
         * \brief Windows macro SetConsoleTitle messes stuff up, so it's name is as below.
         * \param Title Title
         */
        static void SetRealSimConsoleTitle(LPCTSTR Title);

        [[nodiscard]] Graphics::Renderer& GetMainRenderer() const { return *m_Renderer; }
        [[nodiscard]] ECS::Scene& GetCurrentScene() const { return *m_Scene; }
        [[nodiscard]] UpdateStatistics GetStats() const { return m_UpdateStats; }

    private:
        virtual ~App() = default;
    protected:
        virtual ReturnCode OnInit();
        virtual void OnUpdate();
    private:
        /**
         * \param e SDL_Event to be handled
         */
        void Handle_SDL_Events(SDL_Event const& e);
        /**
         * \brief Calculates(sets) the update times/ticks.
         */
        void CalculateUpdateStatistics();
        /**
         * \brief Log the current RealSim core library version.
         */
        static void LogLibraryVersion();

    protected:
        bool m_Running = true;
        UpdateStatistics m_UpdateStats{};

        std::unique_ptr<Window> m_MainWindow;
        std::vector<std::unique_ptr<Window>> m_OtherWindows;

        std::unique_ptr<Graphics::Renderer> m_Renderer;
        Graphics::AssetCache m_AssetCache;

        std::unique_ptr<ECS::Scene> m_Scene;

        std::array<std::array<ECS::Entity,8>,8> m_Monkeys;
        ECS::Entity m_Box;
        ECS::Entity m_Box2;
        ECS::Entity m_Box3;
        ECS::Entity m_Camera;
        ECS::PerspectiveCameraComponent* pCamera{};

        UI::CameraController m_CameraController;
        UI::LayerStack m_LayerStack;

        AssetLib::MeshInfo SusanneMeshInfo;
        AssetLib::AssetRef SusanneAsset;
    };
}

namespace RSim
{
    static Core::App& GetApp()
    {
        return Core::App::Get();
    }
}
