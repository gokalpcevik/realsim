#pragma once

#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl/client.h>

#include "realsim/graphics/GraphicsDevice.h"
#include "realsim/core/Window.h"

namespace RSim::Graphics
{
    class SwapChain
    {
    public:
        SwapChain(IDXGIFactory3* pDXGIFactory, Core::Window const* window, GraphicsDevice const& device);

        [[nodiscard]] Microsoft::WRL::ComPtr<IDXGISwapChain1> const& GetSwapChain1() const;
        [[nodiscard]] IDXGISwapChain1* GetSwapChain1Raw() const;

        [[nodiscard]] UINT GetCurrentBackBufferIndex() const;

        void Present(UINT syncInterval, UINT presentFlags) const;

    private:
        Microsoft::WRL::ComPtr<IDXGISwapChain1> m_DXGISwapChain;
    };
}
