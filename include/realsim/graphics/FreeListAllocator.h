#pragma once
#include <cstdint>
#include <deque>
#include <map>

#include "realsim/core/Assert.h"
#include "realsim/core/Logger.h"

namespace RSim::Graphics
{
	/**
	 * \brief A free list allocator using std::map(red-black trees) for managing variable sized allocations.
	 * Largely inspired(almost the same) by DiligintEngine's VariableSizedAllocationsManager.
	 * Check out for more info: https://diligentgraphics.com/diligent-engine/architecture/d3d12/variable-size-memory-allocations-manager/  
	 */
	class FreeListAllocator
	{
	public:
		using OffsetType = std::size_t;
	private:
		struct FreeBlockInfo;

		using TFreeBlocksByOffsetMap = std::map<OffsetType, FreeBlockInfo>;
		using TFreeBlocksBySizeMap = std::multimap<OffsetType, TFreeBlocksByOffsetMap::iterator>;

		struct FreeBlockInfo
		{
			OffsetType Size;

			TFreeBlocksBySizeMap::iterator OrderBySizeIt{};

			FreeBlockInfo(OffsetType Size) : Size(Size) {}
		};
	protected:
		struct Allocation
		{
			Allocation(OffsetType Offset, OffsetType Size) : Offset{Offset} , Size{Size} {}
			OffsetType Offset;
			OffsetType Size;

			static Allocation InvalidAllocation() { return Allocation{ 0ULL , InvalidOffset()}; }

			[[nodiscard]] bool IsValid() const
			{
				return *this == InvalidAllocation();
			}

			bool operator==(Allocation&& rhs) const noexcept
			{
				return Offset == rhs.Offset && Size == rhs.Size;
			}
		};

	public:
		explicit FreeListAllocator(OffsetType MaxSize);
		FreeListAllocator(FreeListAllocator const&) = delete;
		FreeListAllocator& operator=(FreeListAllocator const&) = delete;
		FreeListAllocator(FreeListAllocator&& rhs) noexcept;
		FreeListAllocator& operator=(FreeListAllocator&& rhs) = default;
		virtual ~FreeListAllocator();

		[[nodiscard]] Allocation Allocate(OffsetType Size);
		void Free(OffsetType Offset, OffsetType Size);
		void Free(Allocation& allocation);

		[[nodiscard]] OffsetType IsFull() const { return m_FreeSize == 0; }
		[[nodiscard]] OffsetType IsEmpty() const { return m_MaxSize==m_FreeSize; }
		[[nodiscard]] OffsetType GetMaxSize() const { return m_MaxSize; }
		/**
		 * \brief Due to fragmentation, allocations can still fail even if the return value is greater than the requested allocation size.
		 */
		[[nodiscard]] OffsetType GetFreeSize() const { return m_FreeSize; }
		[[nodiscard]] std::size_t GetNumFreeBlocks() const { return m_FreeBlocksByOffset.size(); }
		[[nodiscard]] static constexpr OffsetType InvalidOffset() { return std::numeric_limits<OffsetType>::max(); }
	private:
		/**
		 * \brief Add a new free block.
		 */
		void AddNewBlock(OffsetType Offset, OffsetType Size);
	protected:
		TFreeBlocksByOffsetMap m_FreeBlocksByOffset{};
		TFreeBlocksBySizeMap m_FreeBlocksBySize{};
		OffsetType m_MaxSize = 0;
		OffsetType m_FreeSize = 0;
	};

	class FreeListGPUAllocator : public FreeListAllocator
	{
	private:
		struct StaleAllocation
		{
			OffsetType Offset;
			OffsetType Size;
			uint64_t FenceValue;

			StaleAllocation(OffsetType _Offset, OffsetType _Size, uint64_t _FenceValue) :
				Offset( _Offset ), Size(_Size), FenceValue(_FenceValue)
			{}
		};

	public:
		FreeListGPUAllocator(OffsetType MaxSize);
		FreeListGPUAllocator(FreeListGPUAllocator&& rhs) noexcept;
		FreeListGPUAllocator& operator=(FreeListGPUAllocator&& rhs) = default;
		FreeListGPUAllocator(FreeListGPUAllocator const&) = delete;
		FreeListGPUAllocator& operator=(FreeListGPUAllocator const&) = delete;
		~FreeListGPUAllocator() override;

		void Free(OffsetType Offset, OffsetType Size, uint64_t FenceValue);
		void Free(FreeListAllocator::Allocation& allocation, uint64_t FenceValue);

		void ReleaseStaleAllocations(uint64_t LastCompletedFenceValue);

		[[nodiscard]] std::size_t GetStaleAllocationsSize() const { return m_StaleAllocationsSize; }
	private:
		std::deque<StaleAllocation> m_StaleAllocations{};
		size_t m_StaleAllocationsSize = 0;
	};
}