#include "realsim/Graphics/ShaderCompiler.h"

namespace RSim::Graphics
{
	ShaderCompilationParameters ShaderCompilationParameters::VertexShaderDefaults(
		LPCWSTR ShaderPath)
	{
		return { ShaderPath, nullptr,D3D_COMPILE_STANDARD_FILE_INCLUDE,"main","vs_5_1",0,ShaderType::Vertex };
	}

	ShaderCompilationParameters ShaderCompilationParameters::PixelShaderDefaults(
		LPCWSTR ShaderPath)
	{
		return { ShaderPath, nullptr,D3D_COMPILE_STANDARD_FILE_INCLUDE,"main","ps_5_1",0,ShaderType::Pixel };
	}

	ShaderBlob ShaderCompiler::CompileShader(ShaderCompilationParameters const& param)
	{
		rsim_trace("Attempting to compile shader : '{0}'", Utils::WideStringToUTF8(std::wstring(param.ShaderPath)).c_str());
		assert(std::filesystem::exists(param.ShaderPath) && "Shader file doesn't exist!");
		ShaderBlob entry;
		HRESULT result = D3DCompileFromFile(
			param.ShaderPath, 
			param.Defines, 
			param.Include, 
			param.EntryPoint, 
			param.Target, 
			param.Flags,
			0, 
			&entry.Blob, 
			&entry.ErrorBlob);

		if(FAILED(result) && entry.ErrorBlob)
		{
			rsim_trace(static_cast<char const*>(entry.ErrorBlob->GetBufferPointer()));
			return { nullptr,nullptr };
		}

		rsim_trace("Shader compilation succeeded.");
		entry.ShaderType = param.ShaderType;
		entry.CompilationSucceeded = true;
		return entry;
	}
}
