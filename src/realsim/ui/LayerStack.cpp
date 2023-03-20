#include "realsim/editor/LayerStack.h"

namespace RSim::Editor
{
	void LayerStack::OnInit() const
	{
        for (auto* layer : m_pLayers)
        {
            layer->OnInitialize();
        }
	}

	void LayerStack::OnUpdate() const
	{
		for (auto* layer : m_pLayers)
		{
            layer->OnUpdate();
		}
	}

	void LayerStack::OnRenderUI() const
	{
		for (auto* layer : m_pLayers)
		{
            layer->OnRenderUI();
		}
	}

	void LayerStack::OnRender() const
	{
		for (auto* layer : m_pLayers)
		{
            layer->OnRender();
		}
	}

	void LayerStack::OnShutdown() const
	{
		for (auto* layer : m_pLayers)
		{
            layer->OnShutdown();
		    delete layer;
        }
	}
}
