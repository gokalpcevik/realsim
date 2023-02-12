#pragma once
#include <d3dcompiler.h>
#include <wrl.h>
#include <filesystem>

#include "realsim/core/Logger.h"

#include "realsim/graphics/RealSimGraphics.h"
#include "realsim/graphics/Exception.h"

#include "realsim/utils/StringUtils.h"

namespace RSim::Graphics
{
	struct ShaderBlob
	{
		ShaderBlob() = default;

		Microsoft::WRL::ComPtr<ID3DBlob> Blob{nullptr};
		Microsoft::WRL::ComPtr<ID3DBlob> ErrorBlob{nullptr};
		ShaderType ShaderType{ShaderType::Invalid};
		bool CompilationSucceeded = false;

		Microsoft::WRL::ComPtr<ID3DBlob> const& operator->() const { return Blob; }
		operator bool() const { return CompilationSucceeded; }
	};

	struct ShaderCompilationParameters
	{
		LPCWSTR ShaderPath{};
		D3D_SHADER_MACRO const* Defines{nullptr};
		ID3DInclude* Include{ D3D_COMPILE_STANDARD_FILE_INCLUDE };
		LPCSTR EntryPoint = "main";
		LPCSTR Target{};
		UINT Flags{};
		ShaderType ShaderType{ ShaderType::Invalid };

		[[nodiscard]] static ShaderCompilationParameters VertexShaderDefaults(LPCWSTR ShaderPath);
		[[nodiscard]] static ShaderCompilationParameters PixelShaderDefaults(LPCWSTR ShaderPath);
	};

	class ShaderCompiler
	{
	public:
		ShaderCompiler() = default;

		static ShaderBlob CompileShader(ShaderCompilationParameters const& param);
	};

}