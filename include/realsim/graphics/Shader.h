#pragma once
#include <d3d12.h>
#include <d3dx12.h>

#include "realsim/graphics/ShaderCompiler.h"
#include "realsim/graphics/ShaderReflection.h"

namespace RSim::Graphics
{
	class Shader
	{
	public:
		explicit Shader(ShaderCompilationParameters const& params);
		explicit Shader(ShaderBlob blob);
		Shader(Shader const&) = default;
		Shader& operator=(Shader const&) = default;
		Shader(Shader&&) = default;
		Shader& operator=(Shader&&) = default;

		virtual ~Shader() = default;

		[[nodiscard]] virtual ShaderBlob const& GetBlob() const { return m_ShaderBlob; }
		[[nodiscard]] virtual CD3DX12_SHADER_BYTECODE GetByteCode() const;
		[[nodiscard]] virtual ShaderType GetType() const { return m_ShaderBlob.ShaderType; }
		[[nodiscard]] virtual bool CompilationSucceeded() const { return m_ShaderBlob.CompilationSucceeded; }
		[[nodiscard]] virtual ID3DBlob* GetErrorBlob() const { return m_ShaderBlob.ErrorBlob.Get(); }
		[[nodiscard]] virtual std::string_view GetErrorMessage() const;
	protected:
		Shader() = default;

		ShaderBlob m_ShaderBlob;
	};

	class VertexShader final : public Shader
	{
	public:
		/**
		 * \brief Compiles a shader with default vertex shader parameters.
		 */
		explicit VertexShader(LPCWSTR ShaderPath);
		VertexShader(ShaderCompilationParameters const& params);
		VertexShader(ShaderBlob blob);

		[[nodiscard]] VertexLayout const& GetLayout() const { return m_Layout; }
		[[nodiscard]] uint64_t GetNumInputParameters() const { return m_Layout.GetNumInputParameters(); }
		[[nodiscard]] uint64_t GetStride() const { return m_Layout.GetStride(); }
		[[nodiscard]] std::vector<VertexLayoutElement> const& GetElements() const { return m_Layout.GetElements(); }
		[[nodiscard]] std::vector<D3D12_INPUT_ELEMENT_DESC> GetD3D12InputLayout() const { return m_Layout.GetD3D12InputLayout(); }
	private:
		VertexLayout m_Layout;
	};

	class PixelShader final : public Shader
	{
	public:
		/**
		 * \brief Compiles a shader with default pixel shader parameters.
		 */
		explicit PixelShader(LPCWSTR ShaderPath);
		PixelShader(ShaderCompilationParameters const& params);
		PixelShader(ShaderBlob blob);
	};
}