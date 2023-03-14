#pragma once
#include "realsim/editor/Layer.h"


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
		void RenderStatistics(Core::Application* app) const;
	};
	
}