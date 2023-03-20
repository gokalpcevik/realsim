#pragma once
#include "realsim/ui/Layer.h"
#include "realsim/ui/SceneViewWidget.h"
#include "realsim/ecs/Entity.h"
#include "realsim/ecs/Scene.h"
#include "realsim/graphics-de/Renderer.h"

namespace RSim::UI
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
        void DrawStatistics(Graphics::Renderer* renderer) const;
	};
}