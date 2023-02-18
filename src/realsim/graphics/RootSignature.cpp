#include "realsim/graphics/RootSignature.h"
#include "realsim/graphics/RootSignatureFileDeserializer.h"

namespace RSim::Graphics
{
	SerializedRootSignature SerializeRootSignature(CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC const& Desc, ID3D12Device2* pDevice, HRESULT& OutResult, bool HighestVersion)
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

		if (HighestVersion)
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
			if (FAILED(pDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			{
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
			}
		}

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

		OutResult = D3DX12SerializeVersionedRootSignature(&Desc,
		                                      featureData.HighestVersion,
		                                      &rootSignatureBlob,
		                                      &errorBlob);
		return {featureData.HighestVersion, std::move(rootSignatureBlob), std::move(errorBlob)};
	}

	SerializedRootSignature SerializeRootSignature(RootSignatureFileDeserializer const& File, ID3D12Device2* pDevice,
		HRESULT& OutResult, bool HighestVersion)
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

		if (HighestVersion)
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
			if (FAILED(pDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			{
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
			}
		}

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc{};
		auto const& rootParameters = File.GetRootParameters();
		desc.Init_1_1(rootParameters.size(), (D3D12_ROOT_PARAMETER1*)rootParameters.data(), 0, nullptr, File.GetDeserializedRootSignatureFlags());

		return SerializeRootSignature(desc, pDevice, OutResult, HighestVersion);
	}

	RootParameter::RootParameter(CD3DX12_ROOT_PARAMETER1 parameter)
		: m_RootParameter(parameter)
	{
	}

	D3D12_SHADER_VISIBILITY RootParameter::GetShaderVisibility() const
	{
		return m_RootParameter.ShaderVisibility;
	}

	D3D12_ROOT_PARAMETER_TYPE RootParameter::GetParameterType() const
	{
		return m_RootParameter.ParameterType;
	}

	UINT RootParameter::GetShaderRegister(UINT DescriptorRangeIndex) const
	{
		switch (m_RootParameter.ParameterType)
		{
		case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
			{
				RSIM_ASSERT(
					DescriptorRangeIndex > 0 && DescriptorRangeIndex < m_RootParameter.DescriptorTable.
					NumDescriptorRanges);
				return m_RootParameter.DescriptorTable.pDescriptorRanges[DescriptorRangeIndex].BaseShaderRegister;
			}
		case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS: return m_RootParameter.Constants.ShaderRegister;
		case D3D12_ROOT_PARAMETER_TYPE_CBV: return m_RootParameter.Descriptor.ShaderRegister;
		case D3D12_ROOT_PARAMETER_TYPE_SRV: return m_RootParameter.Descriptor.ShaderRegister;
		case D3D12_ROOT_PARAMETER_TYPE_UAV: return m_RootParameter.Descriptor.ShaderRegister;
		}
		return 0;
	}

	UINT RootParameter::GetRegisterSpace(UINT DescriptorRangeIndex) const
	{
		switch (m_RootParameter.ParameterType)
		{
		case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
			{
				RSIM_ASSERT(DescriptorRangeIndex > 0 && DescriptorRangeIndex < m_RootParameter.DescriptorTable.NumDescriptorRanges);
				return m_RootParameter.DescriptorTable.pDescriptorRanges[DescriptorRangeIndex].RegisterSpace;
			}
		case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS: return m_RootParameter.Constants.RegisterSpace;
		case D3D12_ROOT_PARAMETER_TYPE_CBV: return m_RootParameter.Descriptor.RegisterSpace;
		case D3D12_ROOT_PARAMETER_TYPE_SRV: return m_RootParameter.Descriptor.RegisterSpace;
		case D3D12_ROOT_PARAMETER_TYPE_UAV: return m_RootParameter.Descriptor.RegisterSpace;
		}
		return 0;
	}

	UINT RootParameter::GetNum32BitConstants() const
	{
		RSIM_ASSERT(m_RootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS);
		return m_RootParameter.Constants.Num32BitValues;
	}

	UINT RootParameter::GetNumDescriptorRanges() const
	{
		RSIM_ASSERT(m_RootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
		return m_RootParameter.DescriptorTable.NumDescriptorRanges;
	}

	UINT RootParameter::GetNumDescriptors(UINT DescriptorRangeIndex) const
	{
		RSIM_ASSERT(
			DescriptorRangeIndex > 0 && DescriptorRangeIndex < m_RootParameter.DescriptorTable.NumDescriptorRanges);
		RSIM_ASSERT(m_RootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
		return m_RootParameter.DescriptorTable.pDescriptorRanges[DescriptorRangeIndex].NumDescriptors;
	}

	D3D12_DESCRIPTOR_RANGE_TYPE RootParameter::GetDescriptorRangeType(UINT DescriptorRangeIndex) const
	{
		RSIM_ASSERT(
			DescriptorRangeIndex > 0 && DescriptorRangeIndex < m_RootParameter.DescriptorTable.NumDescriptorRanges);
		RSIM_ASSERT(m_RootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
		return m_RootParameter.DescriptorTable.pDescriptorRanges[DescriptorRangeIndex].RangeType;
	}

	D3D12_DESCRIPTOR_RANGE_FLAGS RootParameter::GetDescriptorRangeFlags(UINT DescriptorRangeIndex) const
	{
		RSIM_ASSERT(
			DescriptorRangeIndex > 0 && DescriptorRangeIndex < m_RootParameter.DescriptorTable.NumDescriptorRanges);
		RSIM_ASSERT(m_RootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
		return m_RootParameter.DescriptorTable.pDescriptorRanges[DescriptorRangeIndex].Flags;
	}

	UINT RootParameter::GetOffsetInDescriptorsFromTableStart(UINT DescriptorRangeIndex) const
	{
		RSIM_ASSERT(
			DescriptorRangeIndex > 0 && DescriptorRangeIndex < m_RootParameter.DescriptorTable.NumDescriptorRanges);
		RSIM_ASSERT(m_RootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
		return m_RootParameter.DescriptorTable.pDescriptorRanges[DescriptorRangeIndex].
			OffsetInDescriptorsFromTableStart;
	}

	RootSignature::RootSignature(ID3D12Device2* pDevice, SerializedRootSignature const& RootSigBlob)
	{
		ThrowIfFailed(pDevice->CreateRootSignature(0UL, 
			RootSigBlob.Blob->GetBufferPointer(), 
			RootSigBlob.Blob->GetBufferSize(), 
			IID_PPV_ARGS(&m_RootSignature)));
	}
}
