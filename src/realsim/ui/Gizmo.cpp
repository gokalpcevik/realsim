#include "realsim/ui/Gizmo.h"
#include "realsim/ui/SceneViewWidget.h"

namespace RSim {
    namespace UI
    {
        void Gizmo::HandleTransformationGizmos(ECS::Scene* scene,Core::Window* window)
        {
            auto selectedEntity = SceneViewWidget::GetSelectedEntity();
            if(selectedEntity == ECS::Entity::Null) return;

            ImGui::Begin("Gizmo Options", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing);

            static int modeUI = 0;

            if(ImGui::IsKeyPressed(ImGuiKey_Space))
            {
                modeUI = (int)(bool)!modeUI;
            }
            ImGui::Text("Mode");
            ImGui::RadioButton("Local", &modeUI, 0); ImGui::SameLine();
            ImGui::RadioButton("World", &modeUI, 1);
            ImGuizmo::MODE mode = modeUI == 1 ? ImGuizmo::WORLD : ImGuizmo::LOCAL;

            ImGui::Text("Operation");
            static int operationUI = 0;
            if(ImGui::IsKeyPressed(ImGuiKey_T))
            {
                operationUI = 0;
            }
            else if(ImGui::IsKeyPressed(ImGuiKey_E))
            {
                operationUI = 1;
            }
            else if(ImGui::IsKeyPressed(ImGuiKey_R))
            {
                operationUI = 2;
            }
            ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
            ImGui::RadioButton("Translation", &operationUI, 0); ImGui::SameLine();
            ImGui::RadioButton("Rotation", &operationUI, 1); ImGui::SameLine();
            ImGui::RadioButton("Scale", &operationUI, 2);
            if(operationUI == 0) operation = ImGuizmo::TRANSLATE;
            else if(operationUI == 1) operation = ImGuizmo::ROTATE;
            else if(operationUI == 2) operation = ImGuizmo::SCALE;

            ImGui::End();


            // Viewport
            ImGui::Begin("Viewport");
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 windowSize = ImGui::GetWindowSize();

            ImGuizmo::SetRect(windowPos.x,windowPos.y,windowSize.x,windowSize.y);

            ECS::Entity cameraEntity = scene->GetPrimaryCamera();
            ECS::PerspectiveCameraComponent cameraComp = cameraEntity.GetComponent<ECS::PerspectiveCameraComponent>();

            float aspectRatio = (float)window->GetWidth() / (float)window->GetHeight();

            // World Transform
            Diligent::float4x4 worldTransform = selectedEntity.GetWorldTransform();
            ImGuizmo::Manipulate(cameraComp.GetViewMatrix().Data(),
                                 cameraComp.GetProjectionMatrix(aspectRatio).Data(),
                                 operation,
                                 mode,
                                 worldTransform.Data());

            float translation[3];
            float scale[3];
            float rotation[3];

            // tc is in the world frame. so we need to convert from world to local.
            // World = Local * ParentWorld
            // Local = World * ParentWorld^-1

            auto&& TC = selectedEntity.GetComponent<ECS::TransformComponent>();
            if(selectedEntity.GetParent().IsNull())
            {
                ImGuizmo::DecomposeMatrixToComponents(worldTransform.Data(), translation, rotation, scale);
                TC.Translation = {translation[0],translation[1],translation[2]};
                TC.Rotation = {rotation[0],rotation[1],rotation[2]};
                TC.Scale  = {scale[0],scale[1],scale[2]};
            }
            else
            {
                auto parentWorldTransform = selectedEntity.GetParent().GetWorldTransform();
                Diligent::float4x4 appliedTransform = worldTransform * parentWorldTransform.Inverse();
                ImGuizmo::DecomposeMatrixToComponents(appliedTransform.Data(), translation, rotation, scale);
                TC.Translation = {translation[0],translation[1],translation[2]};
                TC.Rotation = {rotation[0],rotation[1],rotation[2]};
                TC.Scale  = {scale[0],scale[1],scale[2]};
            }

            // \Viewport
            ImGui::End();

        }
    }
}