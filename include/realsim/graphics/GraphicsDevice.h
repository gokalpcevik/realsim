#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include "realsim/core/Logger.h"
#include "realsim/core/Assert.h"

namespace rsim::graphics
{
	class GraphicsDevice
	{
	public:
		GraphicsDevice();


		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Device2> const& GetDevice2() const;
		[[nodiscard]] ID3D12Device2* GetDevice2Raw() const;

		[[nodiscard]] Microsoft::WRL::ComPtr<IDXGIFactory3> const& GetFactory3() const;
		[[nodiscard]] IDXGIFactory3* GetFactory3Raw() const;

		[[nodiscard]] Microsoft::WRL::ComPtr<IDXGIAdapter1> const& GetAdapter1() const;
		[[nodiscard]] IDXGIAdapter1* GetAdapter1Raw() const;

		[[nodiscard]] bool IsTearingSupported() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12Device2> m_Device;
		Microsoft::WRL::ComPtr<IDXGIFactory3> m_DXGIFactory;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> m_DXGIAdapter;

	};
}