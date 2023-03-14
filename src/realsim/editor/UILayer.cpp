#include "realsim/editor/UILayer.h"
#include "realsim/core/Application.h"
#include "realsim/graphics-de/Renderer.h"
#include "realsim/ecs/Scene.h"

namespace RSim::Editor
{
	void UILayer::OnInitialize(Core::Application*)
	{
		
	}

	void UILayer::OnUpdate(Core::Application*, ECS::Scene*)
	{

	}

	void UILayer::OnRender(Core::Application*, Graphics::Renderer*)
	{

	}

	void UILayer::OnRenderUI(Core::Application* app, ECS::Scene* scene, Graphics::Renderer* renderer)
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if(ImGui::MenuItem("Exit"))
					app->Shutdown();
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		RenderStatistics(app);
		auto const view = scene->GetEnTTRegistry().view<ECS::TransformComponent, ECS::MeshComponent,ECS::NameComponent,ECS::Link>();

		auto& io = ImGui::GetIO();

		ImGui::Begin("Scene View");
		for (auto const entity : view)
		{
			auto&& [TC,MC,NC,Link] = scene->GetEnTTRegistry().get<ECS::TransformComponent, ECS::MeshComponent,ECS::NameComponent, ECS::Link>(entity);
			ImGui::PushID((uint64_t)entity);
			
			ImGui::Text("%s", NC.Name.c_str());
			ImGui::Separator();
			ImGui::Text("Transform Component");
			ImGui::DragFloat3("Translation", (float*)(&TC.Translation),0.1f);
			ImGui::DragFloat3("Rotation", (float*)(&TC.Rotation), 0.1f);
			ImGui::DragFloat3("Scale", (float*)(&TC.Scale),0.1f);
			ImGui::Separator();
			ImGui::Text("Mesh Component");
			ImGui::ColorEdit4("Mesh Color", (float*)(&MC.Material.Color));
			ImGui::DragFloat("Shininess", (float*)(&MC.Material.Shininess),0.1f,0.001f,200.0f);
			ImGui::Separator();

			ImGui::PopID();
		}
		ImGui::End();
	}

	void UILayer::OnShutdown(Core::Application*)
	{

	}

	void UILayer::RenderStatistics(Core::Application* app) const
	{
		ImGui::Begin("Stats");
		ImGui::Text("Total(CPU + GPU)");
		ImGui::Separator();
		ImGui::Text("FPS: %0.1f frames/second", app->GetStats().GetFPS());
		ImGui::Text("Frame Time: %0.1fms", app->GetStats().GetFrameTime());
		ImGui::End();
	}
}
