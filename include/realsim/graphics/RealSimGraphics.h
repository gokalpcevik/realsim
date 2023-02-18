#pragma once
#include "GraphicsDevice.h"

namespace RSim
{
	// Short namespace alias of RSim::Graphics for fast typing.
	namespace GFX = Graphics;

	using ShaderId = uint64_t;

	static constexpr D3D12_GPU_VIRTUAL_ADDRESS GPU_VIRTUAL_ADDRESS_NULL = D3D12_GPU_VIRTUAL_ADDRESS(0);

	enum class ShaderType : uint8_t
	{
		Vertex,
		Pixel,
		Compute,
		Hull,
		Geometry,
		Mesh,
		Invalid
	};
}