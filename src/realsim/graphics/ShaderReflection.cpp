#include "realsim/graphics/ShaderReflection.h"

namespace RSim::Graphics
{
	using Microsoft::WRL::ComPtr;

	std::string_view ShaderDataFormatHelper::ShaderDataFormatToString(ShaderDataFormat format)
	{
		switch (format)
		{
		default: return "DXGI_FORMAT_UNKNOWN";
		case ShaderDataFormat::Unknown: return "DXGI_FORMAT_UNKNOWN";
		case ShaderDataFormat::Float32_1: return "DXGI_FORMAT_R32_FLOAT";
		case ShaderDataFormat::Float32_2: return "DXGI_FORMAT_R32G32_FLOAT";
		case ShaderDataFormat::Float32_3: return "DXGI_FORMAT_R32G32B32_FLOAT";
		case ShaderDataFormat::Float32_4: return "DXGI_FORMAT_R32G32B32A32_FLOAT";
		case ShaderDataFormat::UInt32_1: return "DXGI_FORMAT_R32_UINT";
		case ShaderDataFormat::UInt32_2: return "DXGI_FORMAT_R32G32_UINT";
		case ShaderDataFormat::UInt32_3: return "DXGI_FORMAT_R32G32B32_UINT";
		case ShaderDataFormat::UInt32_4: return "DXGI_FORMAT_R32G32B32A32_UINT";
		case ShaderDataFormat::SInt32_1: return "DXGI_FORMAT_R32_SINT";
		case ShaderDataFormat::SInt32_2: return "DXGI_FORMAT_R32G32_SINT";
		case ShaderDataFormat::SInt32_3: return "DXGI_FORMAT_R32G32B32_SINT";
		case ShaderDataFormat::SInt32_4: return "DXGI_FORMAT_R32G32B32A32_SINT";
		}
	}

	DXGI_FORMAT ShaderDataFormatHelper::ShaderDataFormatToDXGIFormat(ShaderDataFormat format)
	{
		switch (format)
		{
		default: return DXGI_FORMAT_UNKNOWN;
		case ShaderDataFormat::Unknown: return DXGI_FORMAT_UNKNOWN;
		case ShaderDataFormat::Float32_1: return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataFormat::Float32_2: return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataFormat::Float32_3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataFormat::Float32_4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataFormat::UInt32_1: return DXGI_FORMAT_R32_UINT;
		case ShaderDataFormat::UInt32_2: return DXGI_FORMAT_R32G32_UINT;
		case ShaderDataFormat::UInt32_3: return DXGI_FORMAT_R32G32B32_UINT;
		case ShaderDataFormat::UInt32_4: return DXGI_FORMAT_R32G32B32A32_UINT;
		case ShaderDataFormat::SInt32_1: return DXGI_FORMAT_R32_SINT;
		case ShaderDataFormat::SInt32_2: return DXGI_FORMAT_R32G32_SINT;
		case ShaderDataFormat::SInt32_3: return DXGI_FORMAT_R32G32B32_SINT;
		case ShaderDataFormat::SInt32_4: return DXGI_FORMAT_R32G32B32A32_SINT;
		}
	}

	UINT ShaderDataFormatHelper::GetShaderDataFormatSizeInBytes(ShaderDataFormat format)
	{
		switch (format)
		{
		default: return 0UL;
		case ShaderDataFormat::Unknown: return 0UL;
		case ShaderDataFormat::Float32_1: return 4UL * 1UL;
		case ShaderDataFormat::Float32_2: return 4UL * 2UL;
		case ShaderDataFormat::Float32_3: return 4UL * 3UL;
		case ShaderDataFormat::Float32_4: return 4UL * 4UL;
		case ShaderDataFormat::UInt32_1: return 4UL * 1UL;
		case ShaderDataFormat::UInt32_2: return 4UL * 2UL;
		case ShaderDataFormat::UInt32_3: return 4UL * 3UL;
		case ShaderDataFormat::UInt32_4: return 4UL * 4UL;
		case ShaderDataFormat::SInt32_1: return 4UL * 1UL;
		case ShaderDataFormat::SInt32_2: return 4UL * 2UL;
		case ShaderDataFormat::SInt32_3: return 4UL * 3UL;
		case ShaderDataFormat::SInt32_4: return 4UL * 4UL;
		}
	}

	VertexLayout::VertexLayout(ShaderBlob const& blob)
	{
		auto const& entry = blob;
		assert(entry.ShaderType == ShaderType::Vertex
			&& "Shader needs to be a vertex shader to be reflected for the vertex layout deduction.");

		ComPtr<ID3D12ShaderReflection> pReflection;

		ThrowIfFailed(D3DReflect(entry.Blob->GetBufferPointer(), entry.Blob->GetBufferSize(), IID_PPV_ARGS(&pReflection)));

		D3D12_SHADER_DESC shaderDesc;
		ThrowIfFailed(pReflection->GetDesc(&shaderDesc));

		m_NumInputParameters = shaderDesc.InputParameters;

		UINT alignedByteOffset = 0;

		for (UINT i = 0; i < m_NumInputParameters; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC signatureParameterDesc;
			ThrowIfFailed(pReflection->GetInputParameterDesc(i, &signatureParameterDesc));

			VertexLayoutElement layoutElement{};

			if (signatureParameterDesc.Mask == 1)
			{
				if (signatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					layoutElement.Format = ShaderDataFormat::UInt32_1;
				}
				else if (signatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					layoutElement.Format = ShaderDataFormat::SInt32_1;
				}
				else if (signatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					layoutElement.Format = ShaderDataFormat::Float32_1;
				}
			}
			else if (signatureParameterDesc.Mask <= 3)
			{
				if (signatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					layoutElement.Format = ShaderDataFormat::UInt32_2;
				}
				else if (signatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					layoutElement.Format = ShaderDataFormat::SInt32_2;
				}
				else if (signatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					layoutElement.Format = ShaderDataFormat::Float32_2;
				}
			}
			else if (signatureParameterDesc.Mask <= 7)
			{
				if (signatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					layoutElement.Format = ShaderDataFormat::UInt32_3;
				}
				else if (signatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					layoutElement.Format = ShaderDataFormat::SInt32_3;
				}
				else if (signatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					layoutElement.Format = ShaderDataFormat::Float32_3;
				}
			}
			else if (signatureParameterDesc.Mask <= 15)
			{
				if (signatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					layoutElement.Format = ShaderDataFormat::UInt32_4;
				}
				else if (signatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					layoutElement.Format = ShaderDataFormat::UInt32_4;
				}
				else if (signatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					layoutElement.Format = ShaderDataFormat::UInt32_4;
				}
			}
			layoutElement.AlignedByteOffset = alignedByteOffset;
			layoutElement.Size = ShaderDataFormatHelper::GetShaderDataFormatSizeInBytes(layoutElement.Format);
			layoutElement.SemanticName = signatureParameterDesc.SemanticName;
			layoutElement.SemanticIndex = signatureParameterDesc.SemanticIndex;
			alignedByteOffset += layoutElement.Size;
			m_Stride += layoutElement.Size;
			m_LayoutElements.push_back(layoutElement);
		}
	}

	std::vector<D3D12_INPUT_ELEMENT_DESC> VertexLayout::GetD3D12InputLayout() const
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
		UINT inputSlot = 0;
		for(auto& element : m_LayoutElements)
		{
			D3D12_INPUT_ELEMENT_DESC desc;
			desc.Format = ShaderDataFormatHelper::ShaderDataFormatToDXGIFormat(element.Format);
			desc.AlignedByteOffset = element.AlignedByteOffset;
			desc.SemanticName = element.SemanticName.c_str();
			desc.SemanticIndex = element.SemanticIndex;
			desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			desc.InputSlot = 0;
			desc.InstanceDataStepRate = 0UL;
			inputLayout.push_back(desc);
		}

		return inputLayout;

	}

	void VertexLayout::PrintVertexLayoutInfo() const
	{
		for(auto &elem : m_LayoutElements)
		{
			rsim_debug("SemanticName:{0}\nSemanticIndex:{1}\nRegister:{2}\nAlignedByteOffset:{3}\nFormat:{4}\nSize:{5}\n",
				elem.SemanticName,elem.SemanticIndex,elem.Register,elem.AlignedByteOffset,
				ShaderDataFormatHelper::ShaderDataFormatToString(elem.Format),elem.Size);
		}
	}
}
