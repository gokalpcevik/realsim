#include "realsim/graphics/UploadBuffer.h"

#include "realsim/graphics/MemoryAllocator.h"

namespace RSim::Graphics
{
	UploadBuffer::UploadBuffer(MemoryAllocator const& MemAllocator, size_t BufferSize, std::wstring const& Name)
		: m_BufferSize(BufferSize)
	{
        D3D12MA::ALLOCATION_DESC allocDesc{};
        allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
        allocDesc.CustomPool = nullptr;
        allocDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;

        // Upload buffers must be 1-dimensional
        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Width = m_BufferSize;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        m_Allocation = MemAllocator.CreateResource(&allocDesc, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
        m_GPUVirtualAddress = m_Allocation->GetResource()->GetGPUVirtualAddress();

#ifndef NDEBUG
        m_Allocation->SetName(Name.c_str());
#else
        (Name);
#endif
	}

	void* UploadBuffer::Map() const
	{
        void* Memory;
        CD3DX12_RANGE const range(0,m_BufferSize);
        m_Allocation->GetResource()->Map(0, &range, &Memory);
        return Memory;
	}

	void UploadBuffer::Unmap(size_t Begin, size_t End) const
	{
        CD3DX12_RANGE const range(Begin, std::min(End, m_BufferSize));
        m_Allocation->GetResource()->Unmap(0, &range);
	}
}
