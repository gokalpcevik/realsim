#pragma once

namespace RSim::UI
{
	class ILayer
	{
	public:
		ILayer() = default;
		virtual ~ILayer() = default;
		virtual void OnInitialize() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnRender() = 0;
		virtual void OnRenderUI() = 0;
		virtual void OnShutdown() = 0;
	};
}