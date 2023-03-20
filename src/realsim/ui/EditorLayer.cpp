#include "realsim/ui/EditorLayer.h"
#include "realsim/core/App.h"

namespace RSim::UI
{
	void EditorLayer::OnInitialize()
	{
		
	}

	void EditorLayer::OnUpdate()
	{

	}

	void EditorLayer::OnRender()
	{

	}

	void EditorLayer::OnRenderUI()
	{
        auto& app = GetApp();
        auto& renderer = app.GetMainRenderer();
        auto& scene = app.GetCurrentScene();

        // We'll move this to a different file when it starts to get large
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if(ImGui::MenuItem("Exit"))
					app.Shutdown();
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

        // This is probably fine here
        DrawStatistics(&renderer);

        SceneViewWidget::DrawSceneHierarchy(&scene);
        SceneViewWidget::DrawInspector(&scene);
	}

    void EditorLayer::OnShutdown()
	{

	}

	void EditorLayer::DrawStatistics(Graphics::Renderer* renderer) const
	{
        auto& app = GetApp();

		ImGui::Begin("Stats");
		ImGui::Text("Total(CPU + GPU)");
		ImGui::Separator();
		ImGui::Text("FPS: %0.1f frames/second", app.GetStats().GetFPS());
		ImGui::Text("Frame Time: %0.1fms", app.GetStats().GetFrameTime());
        static bool VSync = false;
        ImGui::Checkbox("Enable VSync",&VSync);
		ImGui::End();

        renderer->SetVSyncState(static_cast<uint32_t>(VSync));
	}
}
