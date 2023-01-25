#pragma once
#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>

#include "realsim/RealSim.h"
#include "realsim/core/Window.h"
#include "realsim/core/Input.h"
#include "realsim/core/Logger.h"

#include "realsim/graphics/GraphicsDevice.h"
#include "realsim/graphics/SwapChain.h"

#define REALSIM_EXIT_SUCCESS 0
#define REALSIM_EXIT_FAIL (-1)
#define REALSIM_EXIT_CONTINUE (1)

namespace rsim::core
{
	class Application
	{
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

		return_code Run();
        virtual void Shutdown();

        void SetMainWindow(std::unique_ptr<Window> window);
        void AddWindow(std::unique_ptr<Window> window);

    protected:
        virtual void OnUpdate();
    private:
        void HandleSDLEvents(SDL_Event const& e);
        void CalculateUpdateStatistics();

	protected:
        CommandLineArgs m_StartArgs{};
        bool m_Running = true;
        UpdateStatistics m_UpdateStats{};

        std::unique_ptr<Window> m_MainWindow;
        std::vector<std::unique_ptr<Window>> m_OtherWindows;
	};
}
