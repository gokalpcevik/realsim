#pragma once

#include <D3D12MemAlloc.h>
#include <memory>

namespace RSim::Graphics
{
	class MemoryAllocation;
	/**
	 * \brief Do not instantiate this class directly(you cannot).
	 * Create instances through RSim::Graphics::MemoryAllocator instead.
	 */
	class MemoryAllocation final
	{
	public:
		~MemoryAllocation();

		[[nodiscard]] ID3D12Resource* GetResource() const noexcept;
		[[nodiscard]] ID3D12Heap* GetHeap() const noexcept;

		[[nodiscard]] BOOL WasZeroInitialized() const noexcept;


		[[nodiscard]] UINT64 GetOffset() const noexcept;
		[[nodiscard]] UINT64 GetSize() const noexcept;

		[[nodiscard]] LPCWSTR GetName() const noexcept;
		void SetName(LPCWSTR name) const noexcept;

	private:
		MemoryAllocation(D3D12MA::Allocation* pAllocation);
		MemoryAllocation(MemoryAllocation const&) = delete;
		MemoryAllocation& operator=(MemoryAllocation const&) = delete;

		MemoryAllocation(MemoryAllocation&&) = delete;
		MemoryAllocation& operator=(MemoryAllocation&&) = delete;

		friend class MemoryAllocator;
	private:
		D3D12MA::Allocation* m_Allocation{ nullptr };
	};
}
