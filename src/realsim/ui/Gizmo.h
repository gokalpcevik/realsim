#pragma once
#include "realsim/ecs/Entity.h"
#include "realsim/ecs/CommonComponents.h"
#include "realsim/ecs/CameraComponent.h"
#include "realsim/core/Window.h"

#include "BasicMath.hpp"
#include "imgui.h"
#include "ImGuizmo/ImGuizmo.h"

namespace RSim {
    namespace UI {

        class Gizmo
        {
        public:
            Gizmo() = default;
            static void HandleTransformationGizmos(ECS::Scene* scene,Core::Window* window);
        };

    }
}

