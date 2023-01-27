#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "realsim/core/Window.h"
#include "realsim/core/Logger.h"

#include "realsim/graphics/GraphicsDevice.h"
#include "realsim/graphics/SwapChain.h"
#include "realsim/graphics/MemoryAllocator.h"
#include "realsim/graphics/MemoryAllocation.h"

namespace RSim::Graphics
{
	class Renderer
	{
	public:
		Renderer(Core::Window const* outputWindow);

	private:
		std::unique_ptr<GraphicsDevice> m_GfxDevice;
		std::unique_ptr<SwapChain> m_SwapChain;
		std::unique_ptr<MemoryAllocator> m_MemAllocator;
		
	};
}