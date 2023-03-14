#pragma once
#include <memory>
#include "RefCntAutoPtr.hpp"
#include "Buffer.h"
#include "PipelineState.h"
#include "ShaderResourceBinding.h"
#include "Shader.h"

#include "BasicMath.hpp"

namespace RSim::Graphics
{
	namespace Dl = Diligent;
	/**
	 * \brief Drawable structure represents every object that can be rendered or drawn by the renderer. Every object thats needs to be
	 * rendered needs at least a root signature associated with it, a PSO that it may be sharing with other drawables, a vertex shader
	 * and pixel shader. Other shaders may also be in use but we currently don't use them so I shall add them when necessary.
	 */
	struct Drawable
	{
		Dl::RefCntAutoPtr<Dl::IBuffer> VertexBuffer;
		Dl::RefCntAutoPtr<Dl::IBuffer> IndexBuffer;
		// Transform constant buffer
		Dl::RefCntAutoPtr<Dl::IBuffer> TransformationBuffer;
		Dl::RefCntAutoPtr<Dl::IBuffer> PSConstantsBuffer;
		Dl::RefCntAutoPtr<Dl::IBuffer> CameraDataBuffer;
		Dl::RefCntAutoPtr<Dl::IPipelineState> PipelineState;
		Dl::RefCntAutoPtr<Dl::IShaderResourceBinding> ShaderResourceBinding;
	};

	struct TransformationData
	{
		Dl::float4x4 WorldViewProjectionMatrix;
		Dl::float4x4 ModelToWorldMatrix;
	};

	struct CameraData
	{
		Dl::float4 Position;
		Dl::float4 Forward;
	};
}