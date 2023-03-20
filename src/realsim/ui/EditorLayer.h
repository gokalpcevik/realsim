#pragma once
#include "realsim/editor/Layer.h"
#include "realsim/ecs/Entity.h"
#include "realsim/ecs/Scene.h"
#include "realsim/graphics-de/Renderer.h"

namespace RSim::Editor
{
	class EditorLayer : public ILayer
	{
	public:
		EditorLayer() = default;
		void OnInitialize() override;
		void OnUpdate() override;
		void OnRender() override;
		void OnRenderUI() override;
		void OnShutdown() override;

    private:

        void DrawSceneHierarchy(ECS::Scene* scene);
        void DrawInspector(ECS::Scene* scene);
        void SceneHierarchy_DrawChildrenRecursive(ECS::Entity parent, ECS::Scene *scene);
        void DrawStatistics(Graphics::Renderer* renderer) const;

    private:
        ECS::Entity m_SelectedEntity;
	};
}