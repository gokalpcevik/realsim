#include "realsim/graphics/SwapChain.h"

namespace RSim::Graphics
{
	SwapChain::SwapChain(IDXGIFactory3* pDXGIFactory, ID3D12CommandQueue* pCmdQueue, Core::Window const* window,
	                     GraphicsDevice const& device) : m_Device(device)
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
		swapChainDesc.BufferCount = NumFramesInFlight;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

		bool tearingSupported = device.IsTearingSupported();
		rsim_trace("Tearing Supported : {0}", tearingSupported);
		swapChainDesc.Flags = tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		auto const& pDevice = device.GetDevice2();
		HRESULT hr = pDXGIFactory->CreateSwapChainForHwnd(pCmdQueue,
		                                                  hwnd,
		                                                  &swapChainDesc,
		                                                  nullptr,
		                                                  nullptr,
		                                                  &m_DXGISwapChain);
		if (FAILED(hr))
		{
			rsim_error("Call to IDXGIFactory::CreateSwapChainForHwnd failed!");
			throw ComException(hr);
		}
		hr = pDXGIFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
		if (FAILED(hr))
		{
			rsim_error("IDXGIFactory::MakeWindowAssociation failed in file {0}:{1}", __FILE__, __LINE__);
			throw ComException(hr);
		}

		rsim_info("IDXGISwapChain created successfully.");
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

	void SwapChain::CreateBackBuffersFromSwapChain(ID3D12Device* pDevice,
	                                               std::array<ID3D12Resource*, NumFramesInFlight>& pBackBufferResources,
	                                               ID3D12DescriptorHeap* pDescHeap) const
	{
		// Back Buffer Resources
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pDescHeap->GetCPUDescriptorHandleForHeapStart());

		auto const DescriptorHandleIncrementSizes = GetDescriptorHandleIncrementSizes(pDevice);

		for (UINT i = 0; i < NumFramesInFlight; ++i)
		{
			ThrowIfFailed(m_DXGISwapChain->GetBuffer(i, __uuidof(ID3D12Resource),
			                                         reinterpret_cast<void**>(&pBackBufferResources[i])));

			pDevice->CreateRenderTargetView(pBackBufferResources[i], nullptr, rtvHandle);
			rtvHandle.Offset(static_cast<INT>(DescriptorHandleIncrementSizes.RTVIncrementSize));
		}
	}

	void SwapChain::Present(UINT syncInterval, UINT presentFlags) const
	{
		HRESULT hr = m_DXGISwapChain->Present(syncInterval, presentFlags);

		switch (hr)
		{
		default: break;
		case DXGI_ERROR_DEVICE_RESET:
			{
				rsim_error("Error calling SwapChain::Present(). The device failed due to a badly formed command.");
				throw ComException(hr);
			}
		case DXGI_ERROR_DEVICE_HUNG:
			{
				rsim_error(
					"Error calling SwapChain::Present(). The application's device failed due to badly formed commands sent by the application.");
				throw ComException(hr);
			}
		case DXGI_ERROR_DEVICE_REMOVED:
			{
				rsim_error(
					"Error calling SwapChain::Present(). The video card has been physically removed from the system, or a driver upgrade for the video card has occurred. ");
				hr = m_Device->GetDeviceRemovedReason();
				throw ComException(hr);
			}
		}
	}
}
