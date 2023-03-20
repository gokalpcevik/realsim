#include "realsim/graphics-de/Renderer.h"
#include "realsim/graphics-de/Material.h"

namespace RSim::Graphics
{
	using namespace Diligent;

	Renderer::Renderer(Core::Window* pWindow)
		: pWindow(pWindow)
	{
		m_pEngineFactory = GetEngineFactory(m_DeviceType);

		bool success = CreateDeviceAndContextsAndSwapChain(m_DeviceType,
			m_pEngineFactory,
			&m_pRenderDevice,
			&m_pImmediateContext,
			&m_pSwapChain,
			pWindow);
		RSIM_ASSERTM(success && (m_pEngineFactory != nullptr), "Renderer initilization failed.");


		auto& SwpDesc = m_pSwapChain->GetDesc();
		m_pImGuiImplDiligent = InitializeDiligentImGuiImpl(pWindow, m_pRenderDevice, SwpDesc.ColorBufferFormat, SwpDesc.DepthBufferFormat);
		Resize(pWindow->GetWidth(), pWindow->GetHeight());
	}

	Renderer::~Renderer()
	{
	}

#pragma region imgui

	void Renderer::NewImGuiFrame()
	{
		ImGui_ImplSDL2_NewFrame();
		auto& desc = m_pSwapChain->GetDesc();
		GetImGui().NewFrame(desc.Width, desc.Height, desc.PreTransform);
	}

	void Renderer::FinishImGuiFrame()
	{
		GetImGui().Render(m_pImmediateContext);
	}

	void Renderer::BeginImGuiFrame()
	{
		NewImGuiFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
		auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
		auto* pDSV = m_pSwapChain->GetDepthBufferDSV();
		// Clear the default render target
		const float ClearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		m_pImmediateContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f,0.0f });
		ImGui::Begin("Viewport");
		ImGui::Image((ImTextureID)m_pOffScreenSRV, ImGui::GetContentRegionAvail());
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}


#pragma endregion

	void Renderer::Clear(f32 color[])
	{
		ITextureView* views[] = { m_pOffScreenRTV };

		m_pImmediateContext->SetRenderTargets(1u, views, m_pOffScreenDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		// Clear the back buffer
		m_pImmediateContext->ClearRenderTarget(m_pOffScreenRTV, color, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		m_pImmediateContext->ClearDepthStencil(m_pOffScreenDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	}

	void Renderer::Present()
	{
		m_pSwapChain->Present(m_VSyncEnabled);
	}

	void Renderer::RenderScene(ECS::Scene* pScene)
	{
		entt::registry& registry = pScene->GetEnTTRegistry();



		auto const cameraView = registry.view<ECS::PerspectiveCameraComponent>();
		ECS::PerspectiveCameraComponent const* cameraComponent = nullptr;

		if (cameraView.empty())
		{
			rsim_warn("There is no camera to render the scene with.");
			return;
		}

		for (auto const entity : cameraView)
		{
			auto const& cc = cameraView.get<ECS::PerspectiveCameraComponent>(entity);
			if (cc.IsPrimaryCamera)
			{
				cameraComponent = &cc;
				break;
			}
		}

		auto const view = registry.view<ECS::MeshComponent, ECS::TransformComponent, ECS::Link>();


		for (auto const entity : view)
		{
			auto&& [MC, TC, Link] = view.get<ECS::MeshComponent, ECS::TransformComponent, ECS::Link>(entity);

			float fWidth = (float)pWindow->GetWidth();
			float fHeight = (float)pWindow->GetHeight();

			float aspectRatio = fWidth / fHeight;

			ECS::Entity Entity(pScene, entity);

			float4x4 View;
			float4x4 Projection;
			DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&View, cameraComponent->GetViewMatrix());
			DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&Projection, cameraComponent->GetProjectionMatrix(aspectRatio));

			{
                TransformationData data;
                data.WorldViewProjectionMatrix = (Entity.GetWorldTransform() * View * Projection).Transpose();
                data.ModelToWorldMatrix = Entity.GetWorldTransform().Transpose();
				MapHelper<TransformationData> TransformationCBMap(m_pImmediateContext, MC.Drawable->TransformationBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
				*TransformationCBMap = data;
			}

			{
				MapHelper<ECS::MaterialConstants> PSConstantsCBMap(m_pImmediateContext, MC.Drawable->PSConstantsBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
				*PSConstantsCBMap = MC.Material;
			}

			{
				MapHelper<CameraData> CameraCBMap(m_pImmediateContext, MC.Drawable->CameraDataBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
				CameraData data{};
				XMStoreFloat4((DirectX::XMFLOAT4*)&data.Position, cameraComponent->GetCameraPosition());
				XMStoreFloat4((DirectX::XMFLOAT4*)&data.Forward, cameraComponent->GetCameraDirection());
				*CameraCBMap = data;
			}

			// Bind vertex and index buffers
			const Uint64 offset = 0;
			IBuffer* pBuffs[] = { MC.Drawable->VertexBuffer };
			m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offset, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
			m_pImmediateContext->SetIndexBuffer(MC.Drawable->IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

			// Set the pipeline state
			m_pImmediateContext->SetPipelineState(MC.Drawable->PipelineState);
			// Commit shader resources. RESOURCE_STATE_TRANSITION_MODE_TRANSITION mode
			// makes sure that resources are transitioned to required states.
			m_pImmediateContext->CommitShaderResources(MC.Drawable->ShaderResourceBinding, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

			DrawIndexedAttribs DrawAttrs;     // This is an indexed draw call
			DrawAttrs.IndexType = VT_UINT32; // Index type
			DrawAttrs.NumIndices = MC.pMeshInfo.IndexCount;
			// Verify the state of vertex and index buffers
			DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;


            m_pImmediateContext->DrawIndexed(DrawAttrs);
		}
	}

	Drawable* Renderer::CreateDrawable(AssetLib::MeshInfo* pMeshInfo, AssetLib::Asset* pAssetInfo)
	{
		Drawable* drawable = new Drawable();

		GraphicsPipelineStateCreateInfo PSOCreateInfo;

		PSOCreateInfo.PSODesc.Name = "Graphics PSO";
		PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

		// clang-format off
		// This tutorial will render to a single render target
		PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
		// Set render target format which is the format of the swap chain's color buffer
		PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = TEX_FORMAT_RGBA8_UNORM_SRGB;
		// Set depth buffer format which is the format of the swap chain's back buffer
		PSOCreateInfo.GraphicsPipeline.DSVFormat = TEX_FORMAT_D32_FLOAT;
		// Primitive topology defines what kind of primitives will be rendered by this pipeline state
		PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		// Cull back faces
		PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
		// Enable depth testing
		PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
		// clang-format on

		ShaderCreateInfo ShaderCI;
		// Tell the system that the shader source code is in HLSL.
		// For OpenGL, the engine will convert this into GLSL under the hood.
		ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

		// OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
		ShaderCI.Desc.UseCombinedTextureSamplers = true;

		// Create a shader source stream factory to load shaders from files.
		RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
		m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
		ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
		// Create a vertex shader
		RefCntAutoPtr<IShader> pVS;
		{
			ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
			ShaderCI.EntryPoint = "main";
			ShaderCI.Desc.Name = "Mesh VS";
			ShaderCI.FilePath = "shaders/blinnphong.vs.hlsl";
			m_pRenderDevice->CreateShader(ShaderCI, &pVS);

            for(size_t i = 0; i < pVS->GetResourceCount(); ++i)
            {
                ShaderResourceDesc desc;
                pVS->GetResourceDesc(i,desc);
                rsim_trace(desc.Name);
            }

			// Create dynamic uniform buffer that will store our transformation matrix
			// Dynamic buffers can be frequently updated by the CPU
			CreateUniformBuffer(m_pRenderDevice, sizeof(TransformationData), "VS constants CB", &drawable->TransformationBuffer);
		}
		
		// Create a pixel shader
		RefCntAutoPtr<IShader> pPS;
		{
			ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
			ShaderCI.EntryPoint = "main";
			ShaderCI.Desc.Name = "Mesh PS";
			ShaderCI.FilePath = "shaders/blinnphong.ps.hlsl";
			m_pRenderDevice->CreateShader(ShaderCI, &pPS);

			CreateUniformBuffer(m_pRenderDevice, sizeof(ECS::MeshComponent), "PS Constants CB", &drawable->PSConstantsBuffer);
			CreateUniformBuffer(m_pRenderDevice, sizeof(CameraData), "Camera Data CB", &drawable->CameraDataBuffer);
		}

		LayoutElement LayoutElems[] =
		{
			// Attribute 0 - vertex position
			LayoutElement{0, 0, 3, VT_FLOAT32, False},
			// Attribute 1 - vertex normals
			LayoutElement{1, 0, 3, VT_FLOAT32, False},
			// Attribute 2 - mesh color
			LayoutElement{2, 0, 3, VT_FLOAT32, False},
			// Attribute 3 - uv coordinates
			LayoutElement{3, 0, 2, VT_FLOAT32, False}
		};

		PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
		PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

		PSOCreateInfo.pVS = pVS;
		PSOCreateInfo.pPS = pPS;

		// Define variable type that will be used by default
		PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

		m_pRenderDevice->CreateGraphicsPipelineState(PSOCreateInfo, &drawable->PipelineState);

		// Since we did not explicitly specify the type for 'Constants' variable, default
		// type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables never
		// change and are bound directly through the pipeline state object.
		drawable->PipelineState->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Transformation")->Set(drawable->TransformationBuffer);
		drawable->PipelineState->GetStaticVariableByName(SHADER_TYPE_PIXEL, "PSConstants")->Set(drawable->PSConstantsBuffer);
		drawable->PipelineState->GetStaticVariableByName(SHADER_TYPE_PIXEL, "Camera")->Set(drawable->CameraDataBuffer);
		// Create a shader resource binding object and bind all static resources in it
		drawable->PipelineState->CreateShaderResourceBinding(&drawable->ShaderResourceBinding, true);
		AssetLib::UnpackedMesh unpacked;

		UnpackMesh(*pMeshInfo, *pAssetInfo, unpacked);

		drawable->VertexBuffer = CreateVertexBufferFromMeshAsset(m_pRenderDevice, pMeshInfo, &unpacked);
		drawable->IndexBuffer = CreateIndexBufferFromMeshAsset(m_pRenderDevice,pMeshInfo, &unpacked);

		return drawable;
	}

	void Renderer::Resize(u32 width, u32 height)
	{
		m_pSwapChain->Resize(width, height);

		// Create window-size offscreen render target
		TextureDesc RTColorDesc;
		RTColorDesc.Name = "Offscreen render target";
		RTColorDesc.Type = RESOURCE_DIM_TEX_2D;
		RTColorDesc.Width = m_pSwapChain->GetDesc().Width;
		RTColorDesc.Height = m_pSwapChain->GetDesc().Height;
		RTColorDesc.MipLevels = 1;
		RTColorDesc.Format = TEX_FORMAT_RGBA8_UNORM_SRGB;
		// The render target can be bound as a shader resource and as a render target
		RTColorDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_RENDER_TARGET;
		// Define optimal clear value
		RTColorDesc.ClearValue.Format = RTColorDesc.Format;
		RTColorDesc.ClearValue.Color[0] = 0.350f;
		RTColorDesc.ClearValue.Color[1] = 0.350f;
		RTColorDesc.ClearValue.Color[2] = 0.350f;
		RTColorDesc.ClearValue.Color[3] = 1.f;
		RefCntAutoPtr<ITexture> pRTColor;
		m_pRenderDevice->CreateTexture(RTColorDesc, nullptr, &pRTColor);
		// Store the render target view
		m_pOffScreenRTV = pRTColor->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
		m_pOffScreenSRV = pRTColor->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);


		// Create window-size depth buffer
		TextureDesc RTDepthDesc = RTColorDesc;
		RTDepthDesc.Name = "Offscreen depth buffer";
		RTDepthDesc.Format = TEX_FORMAT_D32_FLOAT;
		RTDepthDesc.BindFlags = BIND_DEPTH_STENCIL;
		// Define optimal clear value
		RTDepthDesc.ClearValue.Format = RTDepthDesc.Format;
		RTDepthDesc.ClearValue.DepthStencil.Depth = 1;
		RTDepthDesc.ClearValue.DepthStencil.Stencil = 0;
		RefCntAutoPtr<ITexture> pRTDepth;
		m_pRenderDevice->CreateTexture(RTDepthDesc, nullptr, &pRTDepth);
		// Store the depth-stencil view
		m_pOffScreenDSV = pRTDepth->GetDefaultView(TEXTURE_VIEW_DEPTH_STENCIL);
	}
}
