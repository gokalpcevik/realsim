#pragma once
#include <memory>
#include "imgui.h"
#include "RenderDevice.h"
#include "ImGuiImplDiligent.hpp"

#include "realsim/editor/imgui_impl_sdl.h"
#include "realsim/core/Window.h"

namespace RSim::Graphics
{
	std::unique_ptr<Diligent::ImGuiImplDiligent> InitializeDiligentImGuiImpl(
		Core::Window* pWindow,
		Diligent::IRenderDevice* pDevice, 
		Diligent::TEXTURE_FORMAT BackBufferFmt, 
		Diligent::TEXTURE_FORMAT DepthBufferFmt);

	void StyleColors();
}