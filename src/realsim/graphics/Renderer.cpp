#include "realsim/graphics/Renderer.h"

namespace RSim::Graphics
{
	using Microsoft::WRL::ComPtr;

	uint64_t IncrementAndSignal(ID3D12CommandQueue* pCmdQueue, ID3D12Fence* pFence,
		uint64_t& fenceValue)
	{
		uint64_t fenceValueForSignal = ++fenceValue;
		ThrowIfFailed(pCmdQueue->Signal(pFence, fenceValueForSignal));
		return fenceValueForSignal;
	}
	void WaitForFenceValue(ID3D12Fence* pFence, uint64_t fenceValue, HANDLE fenceEvent)
	{
		if (pFence->GetCompletedValue() < fenceValue)
		{
			ThrowIfFailed(pFence->SetEventOnCompletion(fenceValue, fenceEvent));
			::WaitForSingleObject(fenceEvent, static_cast<DWORD>(std::chrono::milliseconds::max().count()));
		}
	}
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

		ThrowIfFailed(Device()->GetDevice2()->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_CmdQueue)));

		m_CmdListController = std::make_unique<CommandListController>(Device()->GetDevice2Raw());

		if (outputWindow)
		{
			m_SwapChain = std::make_unique<SwapChain>(Device()->GetFactory3Raw(), m_CmdListController->GetGraphicsQueue().GetCommandQueue().Get() , outputWindow, *Device());
		}
		m_MemAllocator = CreateMemoryAllocator(*Device());


		InitVariables();
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

	void Renderer::Present()
	{
		if (!m_pOutputWindow) throw std::exception("Called Renderer::Present with an invalid window.");

		ID3D12GraphicsCommandList2* gfxCmdList = m_CmdList.Get();
		auto const& backBuffer = m_BackBuffers[m_CurrentBackBufferIndex];

		CD3DX12_RESOURCE_BARRIER resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		gfxCmdList->ResourceBarrier(1UL, &resBarrier);

		m_FrameFenceValues[m_CurrentBackBufferIndex] = GfxQueue().ExecuteCommandList(gfxCmdList);

		UINT syncInterval = 1;
		UINT presentFlags = 0;

		SwpChain()->Present(syncInterval, presentFlags);
		m_CurrentBackBufferIndex = SwpChain()->GetCurrentBackBufferIndex();
		m_CmdListController->WaitForFence(m_FrameFenceValues[m_CurrentBackBufferIndex]);
	}

	void Renderer::InitVariables()
	{
		m_DescriptorHandleIncrementSizes = GetDescriptorHandleIncrementSizes(Device()->GetDevice2Raw());

		m_CmdAllocatorPool = std::make_unique<CommandAllocatorPool>(Device()->GetDevice2Raw(), D3D12_COMMAND_LIST_TYPE_DIRECT);

		//--------------------------- Descriptor Heaps ---------------------------------
		D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc;
		rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDescHeapDesc.NumDescriptors = NumFramesInFlight;
		rtvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvDescHeapDesc.NodeMask = 0;

		ThrowIfFailed(Device()->GetDevice2()->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&m_RTVDescriptorHeap)));

		D3D12_DESCRIPTOR_HEAP_DESC dsvDescHeapDesc;
		dsvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvDescHeapDesc.NumDescriptors = 1;
		dsvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvDescHeapDesc.NodeMask = 0;

		ThrowIfFailed(Device()->GetDevice2()->CreateDescriptorHeap(&dsvDescHeapDesc, IID_PPV_ARGS(&m_DSVDescriptorHeap)));

		// Create the command allocators
		for (auto& cmdAllocator : m_CmdAllocators)
		{
			cmdAllocator = m_CmdAllocatorPool->RequestCommandAllocator(m_FrameNumber);
		}


		ThrowIfFailed(Device()->GetDevice2()->CreateCommandList(0, 
			D3D12_COMMAND_LIST_TYPE_DIRECT, 
			m_CmdAllocators[SwpChain()->GetCurrentBackBufferIndex()].Get(), 
			nullptr, 
			IID_PPV_ARGS(&m_CmdList)));

		ThrowIfFailed(m_CmdList->Close());

		if(m_pOutputWindow)
		{
			// Get the resources that SwapChain creates for the HWND and assign them to m_BackBuffers and set the RTV descriptor heap accordingly. 
			SwpChain()->CreateBackBuffersFromSwapChain(Device()->GetDevice2Raw(), m_BackBuffers, m_RTVDescriptorHeap.Get());
			this->ResizeDepthBuffer(m_pOutputWindow->GetWidth(), m_pOutputWindow->GetHeight());
		}
	}

	void Renderer::ClearDepthStencilView(D3D12_CLEAR_FLAGS flags, FLOAT depth) const
	{
		auto dsv = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_CmdList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1UL, &m_ScissorRect);
	}

	void Renderer::ClearRenderTargetView(FLOAT const color[]) const
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			m_CurrentBackBufferIndex,
			m_DescriptorHandleIncrementSizes.RTVIncrementSize);

		m_CmdList->ClearRenderTargetView(rtv, color, 1, &m_ScissorRect);
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		m_CmdListController->Flush();

		for (size_t i = 0; i < NumFramesInFlight; ++i)
		{
			m_BackBuffers[i]->Release();
			m_FrameFenceValues[i] = m_CmdListController->GetGraphicsQueue().GetLastCompletedFenceValue();
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		ThrowIfFailed(SwpChain()->GetSwapChain1()->GetDesc(&swapChainDesc));
		ThrowIfFailed(SwpChain()->GetSwapChain1()->ResizeBuffers(NumFramesInFlight, width, height,
			swapChainDesc.BufferDesc.Format,
			swapChainDesc.Flags));

		m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

		m_Viewport.Width = static_cast<FLOAT>(width);
		m_Viewport.Height = static_cast<FLOAT>(height);
		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		//-----------------------------
		//this->UpdateRenderTargetViews();
		SwpChain()->CreateBackBuffersFromSwapChain(Device()->GetDevice2Raw(), m_BackBuffers, m_RTVDescriptorHeap.Get());
		//-----------------------------
		m_DepthBuffer.reset();
		this->ResizeDepthBuffer(width, height);
	}

	void Renderer::ResizeDepthBuffer(uint32_t width, uint32_t height)
	{
		D3D12_CLEAR_VALUE optimizedClearValue{};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		CD3DX12_HEAP_PROPERTIES depthBufferHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC depthBufferResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_pOutputWindow->GetWidth(), m_pOutputWindow->GetHeight(), 1, 0, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		D3D12MA::ALLOCATION_DESC allocationDesc;
		allocationDesc.CustomPool = nullptr;
		allocationDesc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
		allocationDesc.Flags = D3D12MA::ALLOCATION_FLAG_COMMITTED;
		allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		m_DepthBuffer = MemAllocator()->CreateResource(&allocationDesc, &depthBufferResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &optimizedClearValue);

		D3D12_DEPTH_STENCIL_VIEW_DESC DSVDescription = {};
		DSVDescription.Format = DXGI_FORMAT_D32_FLOAT;
		DSVDescription.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		DSVDescription.Texture2D.MipSlice = 0;
		DSVDescription.Flags = D3D12_DSV_FLAG_NONE;
		Device()->GetDevice2Raw()->CreateDepthStencilView(m_DepthBuffer->GetResource(), &DSVDescription, m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	}
}
