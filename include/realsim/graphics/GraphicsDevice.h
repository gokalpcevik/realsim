#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include "realsim/core/Logger.h"
#include "realsim/core/Assert.h"
#include "realsim/graphics/Exception.h"

namespace RSim::Graphics
{
	/**
	 * \brief A wrapper around the ID3D12Device interface.
	 * Adds convenience functions and contains boilerplate for device creation.
	 */
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

		// This operator overload may be problematic in some cases but since typing Device.GetDevice2()->....... is long and device is frequently used,
		// I think it is justified.
		Microsoft::WRL::ComPtr<ID3D12Device2> const& operator->() const { return m_Device; }

		/**
		 * \brief Checks for tearing support in the factory
		 */
		[[nodiscard]] bool IsTearingSupported() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12Device2> m_Device;
		Microsoft::WRL::ComPtr<IDXGIFactory3> m_DXGIFactory;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> m_DXGIAdapter;

	};
}