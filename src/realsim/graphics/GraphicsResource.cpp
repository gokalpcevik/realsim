#include "realsim/graphics/GraphicsResource.h"

namespace RSim::Graphics
{
	GraphicsResource::GraphicsResource()
		:
		m_UsageState(D3D12_RESOURCE_STATE_COMMON),
		m_GPUVirtualAddress(GPU_VIRTUAL_ADDRESS_NULL),
		m_IsReady(false)
	{
	}

	GraphicsResource::GraphicsResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState)
		:
		m_Resource(pResource),
		m_UsageState(CurrentState),
		m_GPUVirtualAddress(GPU_VIRTUAL_ADDRESS_NULL),
		m_IsReady(false)
	{
	}

	GraphicsResource::~GraphicsResource()
	{
		m_Resource = nullptr;
		m_GPUVirtualAddress = GPU_VIRTUAL_ADDRESS_NULL;
	}

	VertexBuffer::VertexBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES UsageState, uint32_t VertexStride,
	                           uint32_t BufferSize)
		: GraphicsResource(pResource, UsageState)
	{
		m_GPUVirtualAddress = pResource->GetGPUVirtualAddress();
		m_VertexBufferView.BufferLocation = m_GPUVirtualAddress;
		m_VertexBufferView.SizeInBytes = BufferSize;
		m_VertexBufferView.StrideInBytes = VertexStride;
	}

	VertexBuffer::~VertexBuffer()
	{
		m_Resource = nullptr;
		m_GPUVirtualAddress = GPU_VIRTUAL_ADDRESS_NULL;
	}

	IndexBuffer::IndexBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES UsageState, uint32_t SizeInBytes,
	                         DXGI_FORMAT Format)
		:
		GraphicsResource(pResource, UsageState)
	{
		m_GPUVirtualAddress = pResource->GetGPUVirtualAddress();
		m_IndexBufferView.BufferLocation = m_GPUVirtualAddress;
		m_IndexBufferView.SizeInBytes = SizeInBytes;
		m_IndexBufferView.Format = Format;
	}

	IndexBuffer::~IndexBuffer()
	{
		m_Resource = nullptr;
		m_GPUVirtualAddress = GPU_VIRTUAL_ADDRESS_NULL;
	}

	ConstantBuffer::ConstantBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES UsageState, uint32_t BufferSize,
	                               D3D12_CPU_DESCRIPTOR_HANDLE CBVHandle)
		: GraphicsResource(pResource, UsageState), m_CPUHandle(CBVHandle), m_BufferSize(BufferSize)
	{
		m_GPUVirtualAddress = pResource->GetGPUVirtualAddress();
		pResource->Map(0, &CD3DX12_RANGE(0, 0), &m_MappedBufferPtr);
	}

	ConstantBuffer::~ConstantBuffer()
	{
		m_Resource->Unmap(0, &CD3DX12_RANGE(0, 0));
	}

	void ConstantBuffer::SetBufferData(void const* DataPtr, uint32_t BufferSize) const
	{
		std::memcpy(m_MappedBufferPtr, DataPtr, BufferSize);
	}
}
