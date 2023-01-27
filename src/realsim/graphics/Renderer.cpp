#include "realsim/graphics/Renderer.h"

namespace RSim::Graphics
{
	Renderer::Renderer(Core::Window const* outputWindow)
	{
		if(!outputWindow)
		{
			rsim_error("RealSim does not currently support rendering without a valid window.");
			throw std::exception();
		}

		m_GfxDevice = std::make_unique<GraphicsDevice>();
		m_SwapChain = std::make_unique<SwapChain>(m_GfxDevice->GetFactory3Raw(), outputWindow, *m_GfxDevice);
		m_MemAllocator = CreateMemoryAllocator(*m_GfxDevice);
	}
}
