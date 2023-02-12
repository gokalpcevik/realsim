#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <d3d12.h>
#include <d3dx12.h>
#include <mutex>

#include "realsim/graphics/MemoryAllocation.h"
#include "realsim/graphics/GraphicsResource.h"
#include "realsim/graphics/RealSimGraphics.h"

namespace RSim::Graphics
{

	/**
	 * \brief An UploadBuffer is intended for moving data to a default GPU buffer.
	 */
	class UploadBuffer
	{
	public:
		UploadBuffer(MemoryAllocator const& MemAllocator, size_t BufferSize, std::wstring const& Name);
		UploadBuffer(UploadBuffer const&) = delete;
		UploadBuffer& operator=(UploadBuffer const&) = delete;

		void* Map() const;
		void Unmap(size_t Begin = 0, size_t End = -1) const;

		[[nodiscard]] size_t GetBufferSize() const { return m_BufferSize; }
		[[nodiscard]] std::unique_ptr<MemoryAllocation> const& GetAllocation() const { return m_Allocation; }
		[[nodiscard]] ID3D12Resource* GetResource() const { return m_Allocation->GetResource(); }
		[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return m_GPUVirtualAddress; }
	private:
		D3D12_GPU_VIRTUAL_ADDRESS m_GPUVirtualAddress = GPU_VIRTUAL_ADDRESS_NULL;
		size_t m_BufferSize{0};
		std::unique_ptr<MemoryAllocation> m_Allocation;
	};

}