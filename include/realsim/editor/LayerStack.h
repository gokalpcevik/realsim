#pragma once
#include <vector>
#include "Layer.h"

namespace RSim::Core
{
	class Application;
}

namespace RSim::Editor
{
	class LayerStack
	{
	public:
		LayerStack(Core::Application& app) : m_App(app) {}

		void AddLayer(ILayer* pLayer) { m_pLayers.push_back(pLayer); }
		void OnInit() const;
		void OnUpdate(ECS::Scene* scene) const;
		void OnRenderUI(ECS::Scene*, Graphics::Renderer*) const;
		void OnRender(Graphics::Renderer*) const;
		void OnShutdown() const;
	private:
		Core::Application& m_App;
		std::vector<ILayer*> m_pLayers;
	};
}