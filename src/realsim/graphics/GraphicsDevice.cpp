#include "realsim/graphics/GraphicsDevice.h"

namespace rsim::graphics
{
	using Microsoft::WRL::ComPtr;

	GraphicsDevice::GraphicsDevice()
	{
        UINT8 flags = 0U;
#if defined(_DEBUG) || defined(DEBUG)
        ComPtr<ID3D12Debug3> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
        }
        flags = DXGI_CREATE_FACTORY_DEBUG;
#endif
        HRESULT hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_DXGIFactory));

        if (FAILED(hr))
        {
            rsim_error("Error while creating DXGI Factory!");
            assert(false);
        	return;
        }

        ComPtr<IDXGIFactory6> DXGIFactory6;
        hr = m_DXGIFactory->QueryInterface(IID_PPV_ARGS(&DXGIFactory6));

        if (FAILED(hr))
        {
            rsim_error("RealSim requires at least Windows 10, version 1803 or later.");
            assert(false);
        	return;
        }

        bool dedicatedAdapterFound = false;

        // Search for the dedicated video adapter
        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != DXGIFactory6->EnumAdapterByGpuPreference(
	             adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_DXGIAdapter)); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc{};
            hr = m_DXGIAdapter->GetDesc1(&desc);
            if (FAILED(hr))
                continue;
            // Skip the basic driver adapter.
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                continue;

            // Check to see if the dedicated video adapter supports Direct3D12, but do not create the device.
            if (SUCCEEDED(D3D12CreateDevice(m_DXGIAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                dedicatedAdapterFound = true;

            	// Log information about the dedicated video adapter for testing purposes.
                rsim_info("Using the adapter with the properties below:");

            	constexpr size_t outputSize = _countof(desc.Description) + 1;
                auto adapterDescriptionPtr = new char[outputSize];
                size_t charsConverted = 0;
                const wchar_t* inputW = desc.Description;

            	wcstombs_s(&charsConverted, adapterDescriptionPtr, outputSize, inputW, 128);

            	rsim_info("Description: {0}", adapterDescriptionPtr);
                delete[] adapterDescriptionPtr;

            	rsim_info("Dedicated Video Memory: {0}MB", desc.DedicatedVideoMemory / 1024 / 1024);

            	break;
            }
        }

        if (!dedicatedAdapterFound)
        {
            rsim_error("Dedicated video adapter could not be found!");
            return;
        }

        hr = ::D3D12CreateDevice(m_DXGIAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));

        if (FAILED(hr))
        {
            rsim_error("Error while creating the D3D12Device!");
            return;
        }

        /*
         * Reminders from Microsoft Docs about feature levels and numbering systems:
         * • A feature level always includes the functionality of previous or lower feature levels.
         * • All Direct3D 12 drivers will be Feature Level 11_0 or better.
         * • Direct3D versions use a period; for example, Direct3D 12.0.
         * • Shader models use a period; for example, shader model 5.1.
         * • Feature levels use an underscore; for example, feature level 12_0.
         *
         * For more info, check: https://learn.microsoft.com/en-us/windows/win32/direct3d12/hardware-feature-levels
         */

        static constexpr D3D_FEATURE_LEVEL s_FeatureLevels[] =
        {
                D3D_FEATURE_LEVEL_12_1,
                D3D_FEATURE_LEVEL_12_0,
                D3D_FEATURE_LEVEL_11_1,
                D3D_FEATURE_LEVEL_11_0,
        };

        D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
        {
            _countof(s_FeatureLevels), s_FeatureLevels, D3D_FEATURE_LEVEL_11_0
        };

        hr = m_Device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
        if (SUCCEEDED(hr))
        {
            if (static_cast<uint64_t>(featLevels.MaxSupportedFeatureLevel) < static_cast<uint64_t>(D3D_FEATURE_LEVEL_11_0) )
            {
                rsim_error("Specified feature level not supported!");
                return;
            }
        }
        rsim_info("ID3D12Device created successfully.");
	}

	ComPtr<ID3D12Device2> const& GraphicsDevice::GetDevice2() const
	{
		return m_Device;
	}

	ID3D12Device2* GraphicsDevice::GetDevice2Raw() const
	{
		return m_Device.Get();
	}

	ComPtr<IDXGIFactory3> const& GraphicsDevice::GetFactory3() const
	{
		return m_DXGIFactory;
	}

	IDXGIFactory3* GraphicsDevice::GetFactory3Raw() const
	{
		return m_DXGIFactory.Get();
	}

	ComPtr<IDXGIAdapter1> const& GraphicsDevice::GetAdapter1() const
	{
		return m_DXGIAdapter;
	}

	IDXGIAdapter1* GraphicsDevice::GetAdapter1Raw() const
	{
		return m_DXGIAdapter.Get();
	}

	bool GraphicsDevice::IsTearingSupported() const
	{
		BOOL tearingAllowed = FALSE;
		ComPtr<IDXGIFactory5> factory5;
		if (SUCCEEDED(m_DXGIFactory.As(&factory5)))
		{
			if (FAILED(factory5->CheckFeatureSupport( DXGI_FEATURE_PRESENT_ALLOW_TEARING, &tearingAllowed, sizeof(tearingAllowed))))
			{
				tearingAllowed = FALSE;
			}
		}
		return tearingAllowed;
	}
}
