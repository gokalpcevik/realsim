#include "realsim/editor/LayerStack.h"

namespace RSim::Editor
{
	void LayerStack::OnInit() const
	{
		for(auto* layer : m_pLayers)
		{
			layer->OnInitialize(&m_App);
		}
	}

	void LayerStack::OnUpdate(ECS::Scene* scene) const
	{
		for (auto* layer : m_pLayers)
		{
			layer->OnUpdate(&m_App,scene);
		}
	}

	void LayerStack::OnRenderUI(ECS::Scene* scene,Graphics::Renderer* renderer) const
	{
		for (auto* layer : m_pLayers)
		{
			layer->OnRenderUI(&m_App, scene, renderer);
		}
	}

	void LayerStack::OnRender(Graphics::Renderer* renderer) const
	{
		for (auto* layer : m_pLayers)
		{
			layer->OnRender(&m_App,renderer);
		}
	}

	void LayerStack::OnShutdown() const
	{
		for (auto* layer : m_pLayers)
		{
			layer->OnShutdown(&m_App);
		}
	}
}
