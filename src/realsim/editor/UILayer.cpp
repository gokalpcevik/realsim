#include "realsim/editor/UILayer.h"
#include "realsim/core/Application.h"
#include "realsim/graphics-de/Renderer.h"
#include "realsim/ecs/Entity.h"

#define ENTITY_ID(x) (void*)(uint64_t)((x))

namespace RSim::Editor
{
    using namespace ECS;

    ECS::Entity m_SelectedEntity;

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

		RenderStatistics(app,renderer);
	    RenderSceneView(scene);
        RenderInspector(scene);
	}

    void UILayer::RenderSceneView(ECS::Scene* scene)
    {
        entt::registry& reg = scene->GetEnTTRegistry();
        ImGui::Begin("Scene View");

        reg.each([&](auto entity)
                 {
                     ECS::Entity topEntity = {scene, entity};

                     ECS::Link& parentLink = reg.get<ECS::Link>(entity);
                     ECS::NameComponent& name = reg.get<ECS::NameComponent>(entity);
                     ImGuiTreeNodeFlags flags = ((m_SelectedEntity == topEntity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

                     if(parentLink.GetParent() == entt::null)
                     {
                         bool opened = ImGui::TreeNodeEx(ENTITY_ID(entity),flags, "%s", name.Name.c_str());
                         if(ImGui::IsItemClicked())
                         {
                             m_SelectedEntity = {scene,entity};
                         }
                         if(opened)
                         {
                             entt::entity child = parentLink.GetFirstChild();
                             if(child != entt::null)
                             {
                                 SceneHierarchy_DrawChildrenRecursive(topEntity, scene);
                             }
                             ImGui::TreePop();
                         }
                     }
                 });
        ImGui::End();
    }


    void UILayer::SceneHierarchy_DrawChildrenRecursive(Entity parent, Scene *scene)
    {
        Link& parentLink = parent.GetComponent<Link>();
        entt::entity childEntity = parentLink.GetFirstChild();
        Link& childLink = scene->GetComponent<ECS::Link>(childEntity);
        NameComponent& childName = scene->GetComponent<NameComponent>(childEntity);

        ImGuiTreeNodeFlags flags = ((m_SelectedEntity == childEntity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        bool childOpened = ImGui::TreeNodeEx(ENTITY_ID(childEntity),flags,"%s",childName.Name.c_str());
        if(ImGui::IsItemClicked())
        {
            m_SelectedEntity = {scene,childEntity};
        }
        // first child
        if(childOpened)
        {
            entt::entity grandchild = childLink.GetFirstChild();
            if(grandchild != entt::null)
            {
                SceneHierarchy_DrawChildrenRecursive({scene,childEntity},scene);
            }
            ImGui::TreePop();
        }

        entt::entity nextSibling = childLink.GetNextSibling();
        while(nextSibling != entt::null)
        {
            flags = ((m_SelectedEntity == nextSibling) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
            NameComponent& siblingName = scene->GetComponent<NameComponent>(nextSibling);
            Link& siblingLink = scene->GetComponent<Link>(nextSibling);
            bool siblingOpened = ImGui::TreeNodeEx(ENTITY_ID(nextSibling),flags,"%s",siblingName.Name.c_str());
            if(ImGui::IsItemClicked())
            {
                m_SelectedEntity = {scene,nextSibling};
            }
            nextSibling = siblingLink.GetNextSibling();
            if(siblingOpened)
                ImGui::TreePop();
        }
    }

    void UILayer::RenderInspector(ECS::Scene *scene)
    {
        ImGui::Begin("Inspector");
        if(m_SelectedEntity.IsNull())
        {
            ImGui::Text("No entity is selected.");
            ImGui::End();
            return;
        }
        ImGuiIO& io = ImGui::GetIO();
        // Bold, 24.0f pixel size
        ImGui::PushFont(io.Fonts->Fonts[2]);
        NameComponent& name = m_SelectedEntity.GetComponent<NameComponent>();
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth   = ImGui::CalcTextSize(name.Name.c_str()).x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text(name.Name.c_str());
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::PushFont(io.Fonts->Fonts[1]);
        ImGui::Text("Transform Component");
        ImGui::PopFont();

        TransformComponent& transform = m_SelectedEntity.GetComponent<TransformComponent>();
        ImGui::DragFloat3("Translation",(float*)&transform.Translation,0.01f);
        ImGui::DragFloat3("Rotation",(float*)&transform.Rotation,0.01f);
        ImGui::DragFloat3("Scale",(float*)&transform.Scale,0.01f);

        ImGui::End();
    }


    void UILayer::OnShutdown(Core::Application*)
	{

	}

	void UILayer::RenderStatistics(Core::Application* app,Graphics::Renderer* renderer) const
	{
		ImGui::Begin("Stats");
		ImGui::Text("Total(CPU + GPU)");
		ImGui::Separator();
		ImGui::Text("FPS: %0.1f frames/second", app->GetStats().GetFPS());
		ImGui::Text("Frame Time: %0.1fms", app->GetStats().GetFrameTime());
        static bool VSync = true;
        ImGui::Checkbox("Enable VSync",&VSync);
		ImGui::End();

        renderer->SetVSyncState(static_cast<uint32_t>(VSync));
	}


}
