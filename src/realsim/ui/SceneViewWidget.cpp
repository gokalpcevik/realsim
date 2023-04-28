#include "realsim/ui/SceneViewWidget.h"
#include "realsim/core/App.h"

#define ENTITY_ID(x) (void*)(uint64_t)((x))

namespace RSim::UI
{
    using namespace ECS;

    ECS::Entity SceneViewWidget::s_SelectedEntity = ECS::Entity::Null;
    ECS::Entity DroppedEntity = ECS::Entity::Null;

    void SceneViewWidget::DrawSceneHierarchy(ECS::Scene *scene)
    {
        entt::registry& reg = scene->GetEnTTRegistry();
        ImGui::Begin("Scene View");

        reg.each([&](auto entity)
                 {
                     ECS::Entity topEntity = {scene, entity};

                     ECS::Link& parentLink = reg.get<ECS::Link>(entity);
                     ECS::NameComponent& name = reg.get<ECS::NameComponent>(entity);
                     ImGuiTreeNodeFlags flags = ((s_SelectedEntity == topEntity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

                     if(parentLink.GetParent() == entt::null)
                     {
                         entt::entity child = parentLink.GetFirstChild();
                         if(child == entt::null) flags |= ImGuiTreeNodeFlags_Leaf;
                         bool opened = ImGui::TreeNodeEx(ENTITY_ID(entity),flags, "%s", name.Name.c_str());
                         bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

                         // If the item is clicked, set it as selected.
                         if(ImGui::IsItemClicked())
                         {
                             s_SelectedEntity = topEntity;
                         }

                         // Right click context menu on the scene hierarchy panel
                         if(ImGui::BeginPopupContextItem())
                         {
                             if(ImGui::MenuItem("Delete"))
                             {
                             }

                             if(!topEntity.GetParent().IsNull())
                                 if(ImGui::MenuItem("Unparent"))
                                 {
                                     auto local = topEntity.GetWorldTransform();
                                     float translation[3];
                                     float scale[3];
                                     float rotation[3];
                                     ImGuizmo::DecomposeMatrixToComponents(local.Data(), translation, rotation, scale);
                                     auto&& TC = topEntity.GetComponent<ECS::TransformComponent>();
                                     TC.Translation = {translation[0],translation[1],translation[2]};
                                     TC.Rotation = {rotation[0],rotation[1],rotation[2]};
                                     TC.Scale  = {scale[0],scale[1],scale[2]};
                                     topEntity.GetParent().RemoveChild(topEntity);

                                 }
                             ImGui::EndPopup();
                         }

                         if(hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && topEntity != s_SelectedEntity)
                         {
                             ImGui::Separator();
                         }

                         if(hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && topEntity != s_SelectedEntity)
                         {
                             DroppedEntity = topEntity;
                             rsim_trace(DroppedEntity.GetName().Name);

                             auto parentWorld = DroppedEntity.GetWorldTransform();
                             auto newTransform = s_SelectedEntity.GetWorldTransform() * parentWorld.Inverse();
                             float translation[3];
                             float scale[3];
                             float rotation[3];
                             ImGuizmo::DecomposeMatrixToComponents(newTransform.Data(), translation, rotation, scale);
                             auto&& TC = s_SelectedEntity.GetComponent<ECS::TransformComponent>();
                             TC.Translation = {translation[0],translation[1],translation[2]};
                             TC.Rotation = {rotation[0],rotation[1],rotation[2]};
                             TC.Scale  = {scale[0],scale[1],scale[2]};

                             auto parent = s_SelectedEntity.GetParent();
                             if(!parent.IsNull())
                             {
                                 parent.RemoveChild(s_SelectedEntity);
                             }

                             DroppedEntity.AddChild(s_SelectedEntity);
                         }

                         // Expand the child tree
                         if(opened)
                         {
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

    void SceneViewWidget::SceneHierarchy_DrawChildrenRecursive(ECS::Entity parent, ECS::Scene *scene) {
        Link& parentLink = parent.GetComponent<Link>();
        entt::entity childEntity = parentLink.GetFirstChild();
        ECS::Entity child = {scene,childEntity};
        Link& childLink = scene->GetComponent<ECS::Link>(childEntity);
        NameComponent const& childName = scene->GetComponent<NameComponent>(childEntity);

        entt::entity grandchild = childLink.GetFirstChild();

        ImGuiTreeNodeFlags flags = ((s_SelectedEntity == childEntity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        if(childLink.GetNumChildren() == 0) flags |= ImGuiTreeNodeFlags_Leaf;

        bool childOpened = ImGui::TreeNodeEx(ENTITY_ID(childEntity),flags,"%s",childName.Name.c_str());

        if(ImGui::IsItemClicked())
        {
            s_SelectedEntity = {scene,childEntity};
        }

        // Right click context menu on the scene hierarchy panel
        if(ImGui::BeginPopupContextItem())
        {
            if(ImGui::MenuItem("Delete"))
            {
            }

            if(ImGui::MenuItem("Unparent"))
            {
                auto local = child.GetWorldTransform();
                float translation[3];
                float scale[3];
                float rotation[3];
                ImGuizmo::DecomposeMatrixToComponents(local.Data(), translation, rotation, scale);
                auto&& TC = child.GetComponent<ECS::TransformComponent>();
                TC.Translation = {translation[0],translation[1],translation[2]};
                TC.Rotation = {rotation[0],rotation[1],rotation[2]};
                TC.Scale  = {scale[0],scale[1],scale[2]};
                child.GetParent().RemoveChild(child);

            }
            ImGui::EndPopup();
        }

        bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

        if(hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && child != s_SelectedEntity)
        {
            ImGui::Separator();
        }

        if(hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && child != s_SelectedEntity)
        {
            DroppedEntity = child;
            rsim_trace(DroppedEntity.GetName().Name);

            auto parentWorld = DroppedEntity.GetWorldTransform();
            auto newTransform = s_SelectedEntity.GetWorldTransform() * parentWorld.Inverse();
            float translation[3];
            float scale[3];
            float rotation[3];
            ImGuizmo::DecomposeMatrixToComponents(newTransform.Data(), translation, rotation, scale);
            auto&& TC = s_SelectedEntity.GetComponent<ECS::TransformComponent>();
            TC.Translation = {translation[0],translation[1],translation[2]};
            TC.Rotation = {rotation[0],rotation[1],rotation[2]};
            TC.Scale  = {scale[0],scale[1],scale[2]};

            auto parent = s_SelectedEntity.GetParent();
            if(!parent.IsNull())
            {
                parent.RemoveChild(s_SelectedEntity);
            }

            DroppedEntity.AddChild(s_SelectedEntity);
        }
        // first child
        if(childOpened)
        {
            if(grandchild != entt::null)
            {
                SceneHierarchy_DrawChildrenRecursive({scene,childEntity},scene);
            }
            ImGui::TreePop();
        }

        entt::entity nextSibling = childLink.GetNextSibling();
        while(nextSibling != entt::null)
        {
            flags = ((s_SelectedEntity == nextSibling) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
            NameComponent const& siblingName = scene->GetComponent<NameComponent>(nextSibling);
            Link& siblingLink = scene->GetComponent<Link>(nextSibling);
            ECS::Entity sibling = {scene,nextSibling};
            if(siblingLink.GetNumChildren() == 0) flags |= ImGuiTreeNodeFlags_Leaf;

            bool siblingOpened = ImGui::TreeNodeEx(ENTITY_ID(nextSibling),flags,"%s",siblingName.Name.c_str());

            if(ImGui::IsItemClicked())
            {
                s_SelectedEntity = {scene,nextSibling};
            }

            // Right click context menu on the scene hierarchy panel
            if(ImGui::BeginPopupContextItem())
            {
                if(ImGui::MenuItem("Delete"))
                {
                }
                if(ImGui::MenuItem("Unparent"))
                {
                    auto local = sibling.GetWorldTransform();
                    float translation[3];
                    float scale[3];
                    float rotation[3];
                    ImGuizmo::DecomposeMatrixToComponents(local.Data(), translation, rotation, scale);
                    auto&& TC = sibling.GetComponent<ECS::TransformComponent>();
                    TC.Translation = {translation[0],translation[1],translation[2]};
                    TC.Rotation = {rotation[0],rotation[1],rotation[2]};
                    TC.Scale  = {scale[0],scale[1],scale[2]};
                    sibling.GetParent().RemoveChild(sibling);

                }
                ImGui::EndPopup();
            }


            hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

            if(hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && sibling != s_SelectedEntity)
            {
                ImGui::Separator();
            }

            if(hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && sibling != s_SelectedEntity)
            {
                DroppedEntity = sibling;
                rsim_trace(DroppedEntity.GetName().Name);

                auto parentWorld = DroppedEntity.GetWorldTransform();
                auto newTransform = s_SelectedEntity.GetWorldTransform() * parentWorld.Inverse();
                float translation[3];
                float scale[3];
                float rotation[3];
                ImGuizmo::DecomposeMatrixToComponents(newTransform.Data(), translation, rotation, scale);
                auto&& TC = s_SelectedEntity.GetComponent<ECS::TransformComponent>();
                TC.Translation = {translation[0],translation[1],translation[2]};
                TC.Rotation = {rotation[0],rotation[1],rotation[2]};
                TC.Scale  = {scale[0],scale[1],scale[2]};

                auto parent = s_SelectedEntity.GetParent();
                if(!parent.IsNull())
                {
                    parent.RemoveChild(s_SelectedEntity);
                }

                DroppedEntity.AddChild(s_SelectedEntity);
            }

            nextSibling = siblingLink.GetNextSibling();
            if(siblingOpened)
                ImGui::TreePop();

        }
    }

    void SceneViewWidget::DrawInspector(ECS::Scene *scene) {
        ImGui::Begin("Inspector");
        if(s_SelectedEntity.IsNull())
        {
            ImGui::Text("No entity is selected.");
            ImGui::End();
            return;
        }
        auto parent_name = s_SelectedEntity.GetParent().IsNull() ? "No parent" : s_SelectedEntity.GetParent().GetName().Name.c_str();
        auto sibling_count = s_SelectedEntity.GetParent().IsNull() ? 0 : s_SelectedEntity.GetParent().GetLink().GetNumChildren() - 1;
        ImGui::Text("Parent: %s",parent_name);
        ImGui::Text("Children Count: %u", s_SelectedEntity.GetLink().GetNumChildren());
        ImGui::Text("Sibling Count: %u", sibling_count);

        ImGuiIO& io = ImGui::GetIO();
        // Bold, centered, 24.0 pixel size
        ImGui::PushFont(io.Fonts->Fonts[2]);
        NameComponent const& name = s_SelectedEntity.GetComponent<NameComponent>();
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth   = ImGui::CalcTextSize(name.Name.c_str()).x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text("%s", name.Name.c_str());
        ImGui::PopFont();
        ImGui::Separator();

        PerspectiveCameraComponent* camera = scene->TryGet<PerspectiveCameraComponent>(s_SelectedEntity);
        if(camera != nullptr)
        {
            ImGui::PushFont(io.Fonts->Fonts[1]);
            ImGui::Text("Camera Component");
            ImGui::PopFont();

            ImGui::DragFloat("FOV Half Angle", &camera->FOVHalfAngle,0.05f,20.0f,90.0f);
            ImGui::DragFloat("Near Z", &camera->NearZ,0.05f,0.1f,10.0f);
            ImGui::DragFloat("Far Z", &camera->FarZ,0.05f,50.0f,1000.0f);
            ImGui::End();
            return;
        }

        ImGui::PushFont(io.Fonts->Fonts[1]);
        ImGui::Text("Transform Component");
        ImGui::PopFont();

        TransformComponent const& transform = s_SelectedEntity.GetComponent<TransformComponent>();
        ImGui::DragFloat3("Translation",(float*)&transform.Translation,0.01f);
        ImGui::DragFloat3("Rotation",(float*)&transform.Rotation,0.01f);
        ImGui::DragFloat3("Scale",(float*)&transform.Scale,0.01f);

        MeshComponent* meshComponent = scene->TryGet<MeshComponent>(s_SelectedEntity);
        if(meshComponent != nullptr)
        {
            ImGui::PushFont(io.Fonts->Fonts[1]);
            ImGui::Text("Mesh Component");
            ImGui::PopFont();
            ImGui::ColorEdit3("Mesh Color",(float*)&meshComponent->Material.Color);
            ImGui::DragFloat("Shininess",(float*)&meshComponent->Material.Shininess,0.05f,0.1f,256.0f);
        }

        ImGui::End();
    }
}