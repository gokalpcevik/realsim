#pragma once
#include "realsim/editor/Layer.h"

namespace RSim::ECS
{
    class Entity;
}

namespace RSim::Editor
{
	class UILayer : public ILayer
	{
	public:
		UILayer() = default;
		void OnInitialize(Core::Application*) override;
		void OnUpdate(Core::Application*, ECS::Scene*) override;
		void OnRender(Core::Application*, Graphics::Renderer*) override;
		void OnRenderUI(Core::Application* app, ECS::Scene* scene, Graphics::Renderer* renderer) override;
		void OnShutdown(Core::Application*) override;

    private:
        void RenderSceneView(ECS::Scene* scene);
        void RenderInspector(ECS::Scene* scene);
        void SceneHierarchy_DrawChildrenRecursive(ECS::Entity parent, ECS::Scene *scene);
        void RenderStatistics(Core::Application* app,Graphics::Renderer* renderer) const;
	private:
	};
	
}