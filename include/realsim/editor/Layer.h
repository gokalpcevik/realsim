#pragma once

namespace RSim::Core
{
	class Application;
}

namespace RSim::ECS
{
	class Scene;
}

namespace RSim::Graphics
{
	class Renderer;
}

namespace RSim::Editor
{
	class ILayer
	{
	public:
		ILayer() = default;
		virtual ~ILayer() = default;
		virtual void OnInitialize(Core::Application* app) = 0;
		virtual void OnUpdate(Core::Application* app,ECS::Scene* scene) = 0;
		virtual void OnRender(Core::Application* app, Graphics::Renderer* renderer) = 0;
		virtual void OnRenderUI(Core::Application* app, ECS::Scene* scene, Graphics::Renderer* renderer) = 0;
		virtual void OnShutdown(Core::Application* app) = 0;
	};

}