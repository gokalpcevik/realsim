#pragma once
#include <d3d12.h>
#include <d3dx12.h>

#include "realsim/core/Assert.h"

#include "realsim/graphics/Exception.h"

namespace RSim::Graphics
{
	struct SerializedRootSignature;
	class RootSignatureFileDeserializer;

	/**
	 * \brief Serializes a root signature and returns the blobs.
	 * \param pDevice If the highest version should not be selected, this can be nullptr.
	 * \param HighestVersion Serialize with the highest version.
	 */
	SerializedRootSignature SerializeRootSignature(CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC const& Desc, ID3D12Device2* pDevice, HRESULT& OutResult, bool HighestVersion = true);
	SerializedRootSignature SerializeRootSignature(RootSignatureFileDeserializer const& File, ID3D12Device2* pDevice, HRESULT& OutResult, bool HighestVersion = true);


	class RootParameter
	{
	public:
		explicit RootParameter(CD3DX12_ROOT_PARAMETER1 parameter);

		operator CD3DX12_ROOT_PARAMETER1() const { return m_RootParameter; }

		[[nodiscard]] D3D12_SHADER_VISIBILITY GetShaderVisibility() const;
		[[nodiscard]] D3D12_ROOT_PARAMETER_TYPE GetParameterType() const;
		[[nodiscard]] UINT GetShaderRegister(UINT DescriptorRangeIndex = 0) const;
		[[nodiscard]] UINT GetRegisterSpace(UINT DescriptorRangeIndex = 0) const;
		[[nodiscard]] UINT GetNum32BitConstants() const;
		[[nodiscard]] UINT GetNumDescriptorRanges() const;
		[[nodiscard]] UINT GetNumDescriptors(UINT DescriptorRangeIndex = 0) const;
		[[nodiscard]] D3D12_DESCRIPTOR_RANGE_TYPE GetDescriptorRangeType(UINT DescriptorRangeIndex = 0) const;
		[[nodiscard]] D3D12_DESCRIPTOR_RANGE_FLAGS GetDescriptorRangeFlags(UINT DescriptorRangeIndex = 0) const;
		[[nodiscard]] UINT GetOffsetInDescriptorsFromTableStart(UINT DescriptorRangeIndex = 0) const;
		[[nodiscard]] inline CD3DX12_ROOT_PARAMETER1 GetRootParameter() const { return m_RootParameter; }
	private:
		CD3DX12_ROOT_PARAMETER1 m_RootParameter{};
	};


	struct SerializedRootSignature
	{
		D3D_ROOT_SIGNATURE_VERSION Version;
		Microsoft::WRL::ComPtr<ID3DBlob> Blob{ nullptr };
		Microsoft::WRL::ComPtr<ID3DBlob> ErrorBlob{ nullptr };
	};

	class RootSignature
	{
	public:
		RootSignature(ID3D12Device2* pDevice, SerializedRootSignature const& RootSigBlob);

		Microsoft::WRL::ComPtr<ID3D12RootSignature> const& GetRootSignature() const { return m_RootSignature; }
	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
	};
}