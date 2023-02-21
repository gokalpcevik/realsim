#pragma once

#include <d3d12.h>
#include <d3dx12.h>

#include "realsim/core/Logger.h"

#include "realsim/graphics/MemoryAllocator.h"
#include "realsim/graphics/Exception.h"
#include "realsim/graphics/RealSimGraphics.h"

namespace RSim::Graphics
{
	class GraphicsResource
	{
	public:
		GraphicsResource();
		GraphicsResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState);
		virtual ~GraphicsResource();

		[[nodiscard]] ID3D12Resource* GetResource() { return m_Resource.Get(); }
		[[nodiscard]] ID3D12Resource const* GetResource() const { return m_Resource.Get(); }
		[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return m_GPUVirtualAddress; }
		[[nodiscard]] ID3D12Resource** GetAddressOf() { return m_Resource.GetAddressOf(); }
	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource{nullptr};
		D3D12_RESOURCE_STATES m_UsageState;
		D3D12_GPU_VIRTUAL_ADDRESS m_GPUVirtualAddress;
		bool m_IsReady;
	};


	class VertexBuffer final : public GraphicsResource
	{
	public:
		VertexBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES UsageState, uint32_t VertexStride, uint32_t BufferSize);
		~VertexBuffer() override;

		[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW GetView() const { return m_VertexBufferView; }
	private:
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	};

	class IndexBuffer final : public GraphicsResource
	{
	public:
		IndexBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES UsageState, uint32_t SizeInBytes, DXGI_FORMAT Format);
		~IndexBuffer() override;

		[[nodiscard]] D3D12_INDEX_BUFFER_VIEW GetView() const { return m_IndexBufferView; }
	private:
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView{};
	};

	class ConstantBuffer final : public GraphicsResource
	{
	public:
		ConstantBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES UsageState, uint32_t BufferSize, D3D12_CPU_DESCRIPTOR_HANDLE CBVHandle);
		~ConstantBuffer() override;

		void SetBufferData(void const* DataPtr, uint32_t BufferSize) const;

		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetConstantBufferViewHandle() const { return m_CPUHandle; }
	private:
		D3D12_CPU_DESCRIPTOR_HANDLE m_CPUHandle;
		uint32_t m_BufferSize;
		void* m_MappedBufferPtr{};
	};
}