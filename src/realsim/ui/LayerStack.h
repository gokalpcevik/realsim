#pragma once
#include <vector>
#include "Layer.h"

namespace RSim::UI
{
	class LayerStack
	{
	public:
		LayerStack() = default;

		void AddLayer(ILayer* pLayer) { m_pLayers.push_back(pLayer); }
		void OnInit() const;
		void OnUpdate() const;
		void OnRenderUI() const;
		void OnRender() const;
		void OnShutdown() const;
	private:
		std::vector<ILayer*> m_pLayers;
	};
}