#include "realsim/graphics/SwapChain.h"

namespace RSim::Graphics
{
	SwapChain::SwapChain(IDXGIFactory3* pDXGIFactory, Core::Window const* window, GraphicsDevice const& device)
	{
		HWND hwnd = window->GetHWND();

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = window->GetWidth();
		swapChainDesc.Height = window->GetHeight();
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

		bool tearingSupported = device.IsTearingSupported();
		rsim_trace("Tearing supported: {0}", tearingSupported);
		swapChainDesc.Flags = tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		auto const& pDevice = device.GetDevice2();

		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.NodeMask = 0;
		cmdQueueDesc.Priority = 0;
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;


		Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue;

		if(FAILED(pDevice->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue))))
		{
			rsim_error("Error while creating the copy command queue for the swapchain.");
			return;
		}

		HRESULT hr = pDXGIFactory->CreateSwapChainForHwnd(cmdQueue.Get(), hwnd,
			&swapChainDesc, nullptr, nullptr,
			m_DXGISwapChain.ReleaseAndGetAddressOf());

		hr = pDXGIFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
		if (FAILED(hr))
		{
			rsim_error("IDXGIFactory::MakeWindowAssociation failed in file {0}:{1}", __FILE__, __LINE__);
			return;
		}
		else if (SUCCEEDED(hr))
		{
			rsim_info("IDXGISwapChain created successfully.");
			return;
		}
	}

	Microsoft::WRL::ComPtr<IDXGISwapChain1> const& SwapChain::GetSwapChain1() const
	{
		return m_DXGISwapChain;
	}

	IDXGISwapChain1* SwapChain::GetSwapChain1Raw() const
	{
		return m_DXGISwapChain.Get();
	}

	UINT SwapChain::GetCurrentBackBufferIndex() const
	{
		Microsoft::WRL::ComPtr<IDXGISwapChain3> pSwapChain3;
		// Discard the HRESULT here because we already check this in the device constructor(?)
		// TODO: or just strip the check just from non-debug builds.
		m_DXGISwapChain.As(&pSwapChain3);
		return pSwapChain3->GetCurrentBackBufferIndex();
	}

	void SwapChain::Present(UINT syncInterval, UINT presentFlags) const
	{
		m_DXGISwapChain->Present(syncInterval, presentFlags);
	}
}
