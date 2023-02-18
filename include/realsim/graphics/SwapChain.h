#pragma once

#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl/client.h>

#include "realsim/core/Window.h"

#include "realsim/graphics/GraphicsDevice.h"
#include "realsim/graphics/Exception.h"
#include "realsim/graphics/Helpers.h"
#include "realsim/graphics/RendererConfiguration.h"

namespace RSim::Graphics
{
	/**
     * \brief Wrapper around the IDXGISwapChain1 class that adds a few new functionality for ease of use with D3D12.
     */
    class SwapChain
    {
    public:
        SwapChain(IDXGIFactory3* pDXGIFactory, ID3D12CommandQueue* pCmdQueue, Core::Window const* window, GraphicsDevice const& device);

        [[nodiscard]] Microsoft::WRL::ComPtr<IDXGISwapChain1> const& GetSwapChain1() const;
        [[nodiscard]] IDXGISwapChain1* GetSwapChain1Raw() const;

        [[nodiscard]] UINT GetCurrentBackBufferIndex() const;

        void CreateBackBuffersFromSwapChain(ID3D12Device* pDevice, 
            std::array<ID3D12Resource*, NumFramesInFlight>& pBackBufferResources,
            ID3D12DescriptorHeap* pDescHeap) const;

        void Present(UINT syncInterval, UINT presentFlags) const;

    private:
        Microsoft::WRL::ComPtr<IDXGISwapChain1> m_DXGISwapChain;
        GraphicsDevice const& m_Device;
    };
}
