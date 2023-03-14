#pragma once
#include <filesystem>

#include "PipelineState.h"
#include "RefCntAutoPtr.hpp"

namespace RSim::Graphics
{
	namespace Dl = Diligent;
	namespace fs = std::filesystem;

	struct MaterialDescription
	{
		fs::path VertexShaderPath;
		fs::path PixelShaderPath;
	};

	class Material
	{
	public:


	private:
		Dl::RefCntAutoPtr<Dl::IPipelineState> m_PipelineState;
	};
}