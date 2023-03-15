#pragma once

#include "EngineFactory.h"
#include "EngineFactoryD3D12.h"
#include "EngineFactoryVk.h"
#include "EngineFactoryD3D11.h"
#include "EngineFactoryOpenGL.h"

#include "realsim/core/Window.h"
#include "realsim/core/Logger.h"

namespace RSim::Graphics
{
	Diligent::IEngineFactory* GetEngineFactory(Diligent::RENDER_DEVICE_TYPE DeviceType);

	bool CreateDeviceAndContextsAndSwapChain(Diligent::RENDER_DEVICE_TYPE DeviceType, 
		Diligent::IEngineFactory* pFactory,
		Diligent::IRenderDevice** ppDevice, 
		Diligent::IDeviceContext** ppContext,
		Diligent::ISwapChain** ppSwapChain,
		Core::Window* pWindow);

	void SetAdapterIdToDiscreteIfAvailable(Diligent::IEngineFactory* pFactory, Diligent::EngineCreateInfo* pInfo);

	std::string GetEngineName(Diligent::RENDER_DEVICE_TYPE DeviceType);
}