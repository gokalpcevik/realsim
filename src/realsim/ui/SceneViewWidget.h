#pragma once
#include "realsim/ecs/Entity.h"
#include "realsim/ecs/Scene.h"
#include "realsim/graphics-de/Renderer.h"

namespace RSim::UI
{
    class SceneViewWidget
    {
    public:
        static void DrawSceneHierarchy(ECS::Scene* scene);
        static void DrawInspector(ECS::Scene* scene);
        static void SceneHierarchy_DrawChildrenRecursive(ECS::Entity parent, ECS::Scene *scene);

        [[nodiscard]] static ECS::Entity GetSelectedEntity() { return s_SelectedEntity; }
    private:
        static ECS::Entity s_SelectedEntity;
    };
}
