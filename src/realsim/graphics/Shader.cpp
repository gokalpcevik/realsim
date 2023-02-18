#include "realsim/graphics/Shader.h"

namespace RSim::Graphics
{
	Shader::Shader(ShaderCompilationParameters const& params)
		: m_ShaderBlob(ShaderCompiler::CompileShader(params))
	{
	}

	Shader::Shader(ShaderBlob blob)
		: m_ShaderBlob(std::move(blob))
	{
	}

	CD3DX12_SHADER_BYTECODE Shader::GetByteCode() const
	{
		return { m_ShaderBlob.Blob.Get() };
	}

	std::string_view Shader::GetErrorMessage() const
	{
		return { static_cast<const char*>(m_ShaderBlob.ErrorBlob->GetBufferPointer()) };
	}

	VertexShader::VertexShader(LPCWSTR ShaderPath)
	{
		m_ShaderBlob = ShaderCompiler::CompileShader(ShaderCompilationParameters::VertexShaderDefaults(ShaderPath));
		m_Layout = VertexLayout(m_ShaderBlob);
	}

	VertexShader::VertexShader(ShaderCompilationParameters const& params)
		: Shader(params), m_Layout(m_ShaderBlob)
	{
	}

	VertexShader::VertexShader(ShaderBlob blob)
		: Shader(std::move(blob)), m_Layout(m_ShaderBlob)
	{

	}

	PixelShader::PixelShader(LPCWSTR ShaderPath)
	{
		m_ShaderBlob = ShaderCompiler::CompileShader(ShaderCompilationParameters::PixelShaderDefaults(ShaderPath));
	}

	PixelShader::PixelShader(ShaderCompilationParameters const& params) : Shader(params)
	{
	}

	PixelShader::PixelShader(ShaderBlob blob) : Shader(std::move(blob))
	{
	}
}
