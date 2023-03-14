#include "realsim/graphics-de/EngineInitialization.h"

namespace RSim::Graphics
{
    using namespace Diligent;
    using namespace Core;

	IEngineFactory* GetEngineFactory(RENDER_DEVICE_TYPE DeviceType)
	{
        switch (DeviceType)
        {
#if D3D11_SUPPORTED
        case RENDER_DEVICE_TYPE_D3D11:
        {
            EngineD3D11CreateInfo EngineCI;
#    if ENGINE_DLL
            // Load the dll and import GetEngineFactoryD3D11() function
            auto* GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
#    endif
            return GetEngineFactoryD3D11();
        }
#endif


#if D3D12_SUPPORTED
        case RENDER_DEVICE_TYPE_D3D12:
        {
#    if ENGINE_DLL
            // Load the dll and import GetEngineFactoryD3D12() function
            auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
#    endif
            return GetEngineFactoryD3D12();
        }
#endif
#if VULKAN_SUPPORTED
        case RENDER_DEVICE_TYPE_VULKAN:
        {
#    if EXPLICITLY_LOAD_ENGINE_VK_DLL
            // Load the dll and import GetEngineFactoryVk() function
            auto GetEngineFactoryVk = LoadGraphicsEngineVk();
#    endif
            return GetEngineFactoryVk();
        }
#endif
        default:
            rsim_error("Unknown/unsupported device type");
            return nullptr;
        }
	}

	bool CreateDeviceAndContextsAndSwapChain(RENDER_DEVICE_TYPE DeviceType,
        IEngineFactory* pFactory, 
        IRenderDevice** ppDevice, 
        IDeviceContext** ppContext,
        ISwapChain** ppSwapChain,
        Window* pWindow)
	{
        SwapChainDesc SCDesc;
        // Later we can add a post-processing step to gain finer control over the gamma correction.
        // Also there is an open issue about the sRGB bug about ImGui using Diligent's ImGui integration. 
        SCDesc.ColorBufferFormat = TEX_FORMAT_RGBA8_UNORM;
        SCDesc.DepthBufferFormat = TEX_FORMAT_D32_FLOAT;
        FullScreenModeDesc FSMD{};
        NativeWindow window{ pWindow->GetHWND() };
        switch(DeviceType)
		{
        case RENDER_DEVICE_TYPE_D3D11:
	        {
				IEngineFactoryD3D11* pD3D11Factory = static_cast<IEngineFactoryD3D11*>(pFactory);
        		EngineD3D11CreateInfo info;
                SetAdapterIdToDiscreteIfAvailable(pD3D11Factory, &info);
                pD3D11Factory->CreateDeviceAndContextsD3D11(info, ppDevice, ppContext);
                pD3D11Factory->CreateSwapChainD3D11(*ppDevice, *ppContext, SCDesc, FSMD, window, ppSwapChain);
                break;
	        }
        case RENDER_DEVICE_TYPE_D3D12:
	        {
                IEngineFactoryD3D12* pD3D12Factory = static_cast<IEngineFactoryD3D12*>(pFactory);
                pD3D12Factory->LoadD3D12();
                EngineD3D12CreateInfo info;
                SetAdapterIdToDiscreteIfAvailable(pD3D12Factory, &info);
                pD3D12Factory->CreateDeviceAndContextsD3D12(info, ppDevice, ppContext);
                pD3D12Factory->CreateSwapChainD3D12(*ppDevice, *ppContext, SCDesc, FSMD, window, ppSwapChain);
                break;
        }
        case RENDER_DEVICE_TYPE_VULKAN:
	        {
                IEngineFactoryVk* pVkFactory = static_cast<IEngineFactoryVk*>(pFactory);
                EngineVkCreateInfo info;
                SetAdapterIdToDiscreteIfAvailable(pVkFactory, &info);
                pVkFactory->CreateDeviceAndContextsVk(info, ppDevice, ppContext);
                pVkFactory->CreateSwapChainVk(*ppDevice, *ppContext, SCDesc, window, ppSwapChain);
                break;
        }
        case RENDER_DEVICE_TYPE_GL: return false;
        case RENDER_DEVICE_TYPE_UNDEFINED: return false;
        case RENDER_DEVICE_TYPE_GLES: return false;
        case RENDER_DEVICE_TYPE_METAL: return false;
        case RENDER_DEVICE_TYPE_COUNT: return false;
        }
        return true;
	}

	void SetAdapterIdToDiscreteIfAvailable(Diligent::IEngineFactory* pFactory, Diligent::EngineCreateInfo* pInfo)
	{
        Uint32 NumAdapters = 0;
        pFactory->EnumerateAdapters({ 11,1 }, NumAdapters, nullptr);
        std::vector<GraphicsAdapterInfo> Adapters;
        Adapters.resize(NumAdapters);
        pFactory->EnumerateAdapters({ 11,1 }, NumAdapters, Adapters.data());

        for(Uint32 i = 0; i < NumAdapters; ++i)
        {
	        if(Adapters[i].Type == ADAPTER_TYPE_DISCRETE)
	        {
                pInfo->AdapterId = i;
                return;
	        }
        }
	}

	std::string GetEngineName(Diligent::RENDER_DEVICE_TYPE DeviceType)
	{
        switch(DeviceType) { case RENDER_DEVICE_TYPE_UNDEFINED: return "";
        case RENDER_DEVICE_TYPE_D3D11: return "Direct3D11";
        case RENDER_DEVICE_TYPE_D3D12: return "Direct3D12";
        case RENDER_DEVICE_TYPE_GL: return "";
        case RENDER_DEVICE_TYPE_GLES: return "";
        case RENDER_DEVICE_TYPE_VULKAN: return "Vulkan";
        case RENDER_DEVICE_TYPE_METAL: return "";
        case RENDER_DEVICE_TYPE_COUNT: return "";
        }
        return {};
	}
}
