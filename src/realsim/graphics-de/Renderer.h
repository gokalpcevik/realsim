#pragma once
#include "RenderDevice.h"
#include "SwapChain.h"
#include "EngineFactory.h"
#include "DeviceContext.h"
#include "GraphicsUtilities.h"
#include "RefCntAutoPtr.hpp"
#include "MapHelper.hpp"
#include "TextureLoader.h"

#include "realsim/graphics-de/EngineInitialization.h"
#include "realsim/graphics-de/Drawable.h"
#include "realsim/graphics-de/StaticMeshBuffer.h"
#include "realsim/graphics-de/ImGuiHelpers.h"

#include "realsim/ecs/Scene.h"
#include "realsim/ecs/Entity.h"
#include "realsim/ecs/CameraComponent.h"
#include "realsim/ecs/MeshComponent.h"
#include "realsim/ecs/Link.h"

#include "assetlib/AssetLoader.h"
#include "assetlib/MeshLoader.h"

#include "realsim/core/Window.h"
#include "realsim/core/Assert.h"

#include "realsim/BasicTypes.h"

namespace RSim::Graphics
{
	namespace Dl = Diligent;

	class Renderer
	{
	public:
		Renderer(Core::Window* pWindow);
		~Renderer();

		void Clear(f32 color[]);
		void Present();
		void RenderScene(ECS::Scene* pScene);

		[[nodiscard]] std::string GetEngineName() const { return Graphics::GetEngineName(m_DeviceType); }
		[[nodiscard]] Dl::ImGuiImplDiligent& GetImGui() const { return *m_pImGuiImplDiligent; }
		void BeginImGuiFrame();
		void FinishImGuiFrame();
		void NewImGuiFrame();

		Drawable* CreateDrawable(AssetLib::MeshInfo* pMeshInfo, AssetLib::Asset* pAssetInfo);

        void SetVSyncState(uint32_t Enable) { m_VSyncEnabled = Enable; }
		void Resize(u32 width,u32 height);
	private:
		uint32_t m_VSyncEnabled = 1;

		Core::Window* pWindow;

		Dl::RENDER_DEVICE_TYPE m_DeviceType = Dl::RENDER_DEVICE_TYPE_D3D12;

		Dl::RefCntAutoPtr<Dl::IRenderDevice> m_pRenderDevice;
		Dl::RefCntAutoPtr<Dl::ISwapChain> m_pSwapChain;
		Dl::RefCntAutoPtr<Dl::IDeviceContext> m_pImmediateContext;

		Dl::RefCntAutoPtr<Dl::ITextureView> m_pOffScreenRTV;
		Dl::RefCntAutoPtr<Dl::ITextureView> m_pOffScreenSRV;
		Dl::RefCntAutoPtr<Dl::ITextureView> m_pOffScreenDSV;

		Dl::IEngineFactory* m_pEngineFactory;
		std::unique_ptr<Dl::ImGuiImplDiligent> m_pImGuiImplDiligent;
	};
}