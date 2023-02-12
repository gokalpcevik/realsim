#include "realsim/graphics/Renderer.h"

#include "realsim/graphics/RootSignatureFileDeserializer.h"

namespace RSim::Graphics
{
	using Microsoft::WRL::ComPtr;

	Renderer::Renderer(Core::Window const* outputWindow) : m_pOutputWindow(outputWindow)
	{
		if(!outputWindow)
		{
			rsim_error("RealSim does not currently support rendering without a valid window.");
			throw std::exception();
		}

		m_GfxDevice = std::make_unique<GraphicsDevice>();

		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.NodeMask = 0;
		cmdQueueDesc.Priority = 0;
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		ThrowIfFailed(Device()->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_CmdQueue)));

		m_CmdListController = std::make_unique<CommandListController>(Device().GetDevice2Raw());

		if (outputWindow)
		{
			m_SwapChain = std::make_unique<SwapChain>(Device().GetFactory3Raw(), m_CmdListController->GetGraphicsQueue().GetCommandQueue().Get() , outputWindow, Device());
		}
		m_MemAllocator = CreateMemoryAllocator(Device());
		InitVariables();
		InitRenderingVar();
	}

	void Renderer::Clear(FLOAT const color[]) const
	{
		auto const& cmdAllocator = m_CmdAllocators[m_CurrentBackBufferIndex];
		auto const& backBuffer = m_BackBuffers[m_CurrentBackBufferIndex];
		auto const& cmdList = m_CmdList;

		ThrowIfFailed(cmdAllocator->Reset());
		ThrowIfFailed(cmdList->Reset(cmdAllocator.Get(), nullptr));

		CD3DX12_RESOURCE_BARRIER resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		cmdList->ResourceBarrier(1UL, &resBarrier);

		ClearRenderTargetView(color);
		ClearDepthStencilView(D3D12_CLEAR_FLAG_DEPTH, 1.0f);
	}

	void Renderer::Render(ECS::Scene& Scene) const
	{
		ID3D12GraphicsCommandList2* gfxCmdList = m_CmdList.Get();
		CD3DX12_CPU_DESCRIPTOR_HANDLE const rtv(
			m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), (INT)m_CurrentBackBufferIndex, m_DescriptorSizes.RTVIncrementSize);
		D3D12_CPU_DESCRIPTOR_HANDLE const dsv{ m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

		gfxCmdList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

		auto& registry = Scene.GetEnTTRegistry();


		auto const cameraView = Scene.GetEnTTRegistry().view<ECS::PerspectiveCameraComponent>();
		ECS::PerspectiveCameraComponent const* cameraComponent = nullptr;

		if (cameraView.size() == 0)
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
		gfxCmdList->RSSetViewports(1, &m_Viewport);
		gfxCmdList->RSSetScissorRects(1, &m_ScissorRect);
		auto const view = registry.view<ECS::BoxComponent>();

		for(auto const entity : view)
		{
			auto&&BC = view.get<ECS::BoxComponent>(entity);

			float fWidth = (float)m_pOutputWindow->GetWidth();
			float fHeight = (float)m_pOutputWindow->GetHeight();

			float aspectRatio = fWidth / fHeight;
			ECS::TransformComponent TC{};
			//TC.Translation.z = 1.0f;
			//TC.Translation.x = (BC.ScreenPosition.x / fWidth - 0.5f) * 2.0f;
			//TC.Translation.y = (0.5f - BC.ScreenPosition.y / fHeight) * 2.0f;
			TC.Scale = { 1.0f,1.0f,1.0f };

			DirectX::XMStoreFloat4(&TC.Rotation, DirectX::XMQuaternionRotationRollPitchYaw(0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f));
			DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(TC.GetTransform(), cameraComponent->GetViewMatrix());
			mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, cameraComponent->GetProjectionMatrix(aspectRatio));

			gfxCmdList->SetPipelineState(m_PipelineState.Get());
			gfxCmdList->SetGraphicsRootSignature(m_Sig->GetRootSignature().Get());
			gfxCmdList->IASetVertexBuffers(0UL, 1UL, &m_VB->GetView());
			gfxCmdList->IASetIndexBuffer(&m_IB->GetView());

			gfxCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			gfxCmdList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &mvpMatrix, 0);
			gfxCmdList->SetGraphicsRoot32BitConstants(1, sizeof(DirectX::XMFLOAT4) / 4, &BC.Color, 0);
			gfxCmdList->DrawIndexedInstanced(indices.size(), 1, 0, 0, 0);
		}
	}

	void Renderer::Present()
	{
		if (!m_pOutputWindow) throw std::exception("Called Renderer::Present with an invalid window.");

		ID3D12GraphicsCommandList2* gfxCmdList = m_CmdList.Get();
		auto const& backBuffer = m_BackBuffers[m_CurrentBackBufferIndex];

		CD3DX12_RESOURCE_BARRIER resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		gfxCmdList->ResourceBarrier(1UL, &resBarrier);

		m_FrameFenceValues[m_CurrentBackBufferIndex] = GfxQueue().ExecuteCommandList(gfxCmdList);

		bool EnableVSync = true;
		UINT syncInterval = EnableVSync ? 1 : 0;
		UINT presentFlags = Device().IsTearingSupported() && !EnableVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;

		SwpChain().Present(syncInterval, presentFlags);
		m_CurrentBackBufferIndex = SwpChain().GetCurrentBackBufferIndex();

		m_CmdListController->WaitForFence(m_FrameFenceValues[m_CurrentBackBufferIndex]);
	}

	void Renderer::InitVariables()
	{
		m_DescriptorSizes = GetDescriptorHandleIncrementSizes(Device().GetDevice2Raw());

		m_CmdAllocatorPool = std::make_unique<CommandAllocatorPool>(Device().GetDevice2Raw(), D3D12_COMMAND_LIST_TYPE_DIRECT);

		//--------------------------- Descriptor Heaps ---------------------------------
		D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc;
		rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDescHeapDesc.NumDescriptors = NumFramesInFlight;
		rtvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvDescHeapDesc.NodeMask = 0;

		ThrowIfFailed(Device()->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&m_RTVDescriptorHeap)));

		D3D12_DESCRIPTOR_HEAP_DESC dsvDescHeapDesc;
		dsvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvDescHeapDesc.NumDescriptors = 1;
		dsvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvDescHeapDesc.NodeMask = 0;

		ThrowIfFailed(Device()->CreateDescriptorHeap(&dsvDescHeapDesc, IID_PPV_ARGS(&m_DSVDescriptorHeap)));

		// Create the command allocators
		for (auto& cmdAllocator : m_CmdAllocators)
		{
			cmdAllocator = m_CmdAllocatorPool->RequestCommandAllocator(m_FrameNumber);
		}
		ThrowIfFailed(Device()->CreateCommandList(0, 
			D3D12_COMMAND_LIST_TYPE_DIRECT, 
			m_CmdAllocators[SwpChain().GetCurrentBackBufferIndex()].Get(), 
			nullptr, 
			IID_PPV_ARGS(&m_CmdList)));

		ThrowIfFailed(m_CmdList->Close());
		
		if(m_pOutputWindow)
		{
			// Get the resources that SwapChain creates for the HWND and assign them to m_BackBuffers and set the RTV descriptor heap accordingly. 
			SwpChain().CreateBackBuffersFromSwapChain(Device().GetDevice2Raw(), m_BackBuffers, m_RTVDescriptorHeap.Get());
			this->ResizeDepthBuffer(m_pOutputWindow->GetWidth(), m_pOutputWindow->GetHeight());
		}
	}

	void Renderer::InitRenderingVar()
	{
		D3D12MA::ALLOCATION_DESC intermediateAllocDesc;
		intermediateAllocDesc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
		intermediateAllocDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
		intermediateAllocDesc.CustomPool = NULL;
		intermediateAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		D3D12MA::ALLOCATION_DESC destAllocDesc;
		destAllocDesc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
		destAllocDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
		destAllocDesc.CustomPool = NULL;
		destAllocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		auto verticesResDesc = CD3DX12_RESOURCE_DESC::Buffer(vertices.size() * sizeof(DirectX::XMFLOAT3));
		auto indicesResDesc = CD3DX12_RESOURCE_DESC::Buffer(indices.size() * sizeof(uint32_t));

		m_VBResource = MemAllocator().CreateResource(&destAllocDesc,
			&verticesResDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr);

		m_IBResource = MemAllocator().CreateResource(&destAllocDesc,
			&indicesResDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr);

		auto VBIntermediate = MemAllocator().CreateResource(&intermediateAllocDesc,
			&verticesResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr);

		auto IBIntermediate = MemAllocator().CreateResource(&intermediateAllocDesc,
			&indicesResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr);

		D3D12_SUBRESOURCE_DATA verticesSubresourceData{};
		verticesSubresourceData.pData = vertices.data();
		verticesSubresourceData.RowPitch = vertices.size() * sizeof(DirectX::XMFLOAT3);
		verticesSubresourceData.SlicePitch = verticesSubresourceData.RowPitch;

		D3D12_SUBRESOURCE_DATA indicesSubresourceData{};
		indicesSubresourceData.pData = indices.data();
		indicesSubresourceData.RowPitch = indices.size() * sizeof(uint32_t);
		indicesSubresourceData.SlicePitch = indicesSubresourceData.RowPitch;

		Device()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_CopyCmdAllocator));
		Device()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_CopyCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CopyCmdList));

		::UpdateSubresources(m_CopyCmdList.Get(),m_VBResource->GetResource(),VBIntermediate->GetResource(),0,0,1,&verticesSubresourceData);
		::UpdateSubresources(m_CopyCmdList.Get(), m_IBResource->GetResource(), IBIntermediate->GetResource(), 0, 0, 1, &indicesSubresourceData);

		uint64_t signal = CopyQueue().ExecuteCommandList(m_CopyCmdList.Get());
		CopyQueue().WaitForFence(signal);
#pragma warning(disable: 4267)
		m_VB = std::make_unique<VertexBuffer>(m_VBResource->GetResource(), D3D12_RESOURCE_STATE_COMMON, (uint32_t)sizeof(DirectX::XMFLOAT3), (uint32_t)vertices.size() * (uint32_t)sizeof(DirectX::XMFLOAT3));
		m_IB = std::make_unique<IndexBuffer>(m_IBResource->GetResource(), D3D12_RESOURCE_STATE_COMMON, indices.size() * sizeof(uint32_t), DXGI_FORMAT_R32_UINT);
#pragma warning(default: 4267)

		ShaderBlob VS = ShaderCompiler::CompileShader(ShaderCompilationParameters::VertexShaderDefaults(L"Shaders/vertex.vsh"));
		ShaderBlob PS = ShaderCompiler::CompileShader(ShaderCompilationParameters::PixelShaderDefaults(L"Shaders/pixel.psh"));

		VertexLayout layout(VS);

		RootSignatureFileDeserializer file("RootSignatures/basic.yml");

		HRESULT hr;
		SerializedRootSignature blob = SerializeRootSignature(file, Device().GetDevice2Raw(), hr);
		m_Sig = std::make_unique<RootSignature>(Device().GetDevice2Raw(), blob);

		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		} pipelineStateStream;

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		auto inputLayout = layout.GetD3D12InputLayout();
		pipelineStateStream.pRootSignature = m_Sig->GetRootSignature().Get();
		pipelineStateStream.InputLayout = { inputLayout.data(), static_cast<UINT>(inputLayout.size()) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(VS.Blob.Get());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(PS.Blob.Get());
		pipelineStateStream.RTVFormats = rtvFormats;
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(PipelineStateStream), &pipelineStateStream };

		Device()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState));
	}

	void Renderer::ClearDepthStencilView(D3D12_CLEAR_FLAGS flags, FLOAT depth) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_CmdList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1UL, &m_ScissorRect);
	}

	void Renderer::ClearRenderTargetView(FLOAT const color[]) const
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			(INT)m_CurrentBackBufferIndex,
			m_DescriptorSizes.RTVIncrementSize);

		m_CmdList->ClearRenderTargetView(rtv, color, 1, &m_ScissorRect);
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		m_CmdListController->Flush();

		for (size_t i = 0; i < NumFramesInFlight; ++i)
		{
			m_BackBuffers[i]->Release();
			m_FrameFenceValues[i] = m_CmdListController->GetGraphicsQueue().GetNextFenceValue();
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		ThrowIfFailed(SwpChain().GetSwapChain1()->GetDesc(&swapChainDesc));
		ThrowIfFailed(SwpChain().GetSwapChain1()->ResizeBuffers(NumFramesInFlight, width, height, swapChainDesc.BufferDesc.Format,swapChainDesc.Flags));

		m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

		m_Viewport.Width = static_cast<FLOAT>(width);
		m_Viewport.Height = static_cast<FLOAT>(height);
		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;

		SwpChain().CreateBackBuffersFromSwapChain(Device().GetDevice2Raw(), m_BackBuffers, m_RTVDescriptorHeap.Get());
		m_DepthBuffer.reset();
		this->ResizeDepthBuffer(width, height);
	}

	void Renderer::ResizeDepthBuffer(uint32_t width, uint32_t height)
	{
		D3D12_CLEAR_VALUE optimizedClearValue{};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		CD3DX12_RESOURCE_DESC depthBufferResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		D3D12MA::ALLOCATION_DESC allocationDesc;
		allocationDesc.CustomPool = nullptr;
		allocationDesc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
		allocationDesc.Flags = D3D12MA::ALLOCATION_FLAG_COMMITTED;
		allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		m_DepthBuffer = MemAllocator().CreateResource(&allocationDesc, &depthBufferResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &optimizedClearValue);

		D3D12_DEPTH_STENCIL_VIEW_DESC DSVDescription;
		DSVDescription.Format = DXGI_FORMAT_D32_FLOAT;
		DSVDescription.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		DSVDescription.Texture2D.MipSlice = 0;
		DSVDescription.Flags = D3D12_DSV_FLAG_NONE;
		Device()->CreateDepthStencilView(m_DepthBuffer->GetResource(), &DSVDescription, m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	}
}
