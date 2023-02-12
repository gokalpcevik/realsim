#include "realsim/graphics/FreeListAllocator.h"

namespace RSim::Graphics
{
	FreeListAllocator::FreeListAllocator(OffsetType MaxSize)
		: m_FreeSize(MaxSize)
	{
		AddNewBlock(0ULL, MaxSize);
	}

	void FreeListAllocator::AddNewBlock(OffsetType Offset, OffsetType Size)
	{
		auto NewBlockIt = m_FreeBlocksByOffset.emplace(Offset, Size);
		auto OrderIt = m_FreeBlocksBySize.emplace(Size, NewBlockIt.first);
		NewBlockIt.first->second.OrderBySizeIt = OrderIt;
	}

	FreeListAllocator::FreeListAllocator(FreeListAllocator&& rhs) noexcept
		:
		m_FreeBlocksByOffset(std::move(rhs.m_FreeBlocksByOffset)),
		m_FreeBlocksBySize(std::move(rhs.m_FreeBlocksBySize)),
		m_MaxSize(rhs.m_MaxSize),
		m_FreeSize(rhs.m_FreeSize)
	{
		rhs.m_FreeSize = 0;
		rhs.m_MaxSize = 0;
	}

	FreeListAllocator::~FreeListAllocator()
	{
		if (m_FreeSize != m_MaxSize)
			rsim_warn("FreeListAllocator::~FreeListAllocator: Some allocations may not have been freed.");
	}

	FreeListAllocator::Allocation FreeListAllocator::Allocate(OffsetType Size)
	{
		RSIM_ASSERT(Size > 0);
		auto SmallestBlockItIt = m_FreeBlocksBySize.lower_bound(Size);
		if (SmallestBlockItIt == m_FreeBlocksBySize.end()) 
			return Allocation::InvalidAllocation();

		auto SmallestBlockIt = SmallestBlockItIt->second;
		auto Offset = SmallestBlockIt->first;

		auto NewOffset = Offset + Size;
		auto NewSize = SmallestBlockIt->second.Size - Size;

		m_FreeBlocksBySize.erase(SmallestBlockItIt);
		m_FreeBlocksByOffset.erase(SmallestBlockIt);
		if(NewSize > 0)
		{
			AddNewBlock(NewOffset, NewSize);
		}

		return { Offset, Size };
	}

	void FreeListAllocator::Free(OffsetType Offset, OffsetType Size)
	{
		auto NextBlockIt = m_FreeBlocksByOffset.upper_bound(Offset);
		auto PrevBlockIt = NextBlockIt;

		if (PrevBlockIt != m_FreeBlocksByOffset.begin())
			--PrevBlockIt;
		else
			PrevBlockIt = m_FreeBlocksByOffset.end();

		OffsetType NewSize, NewOffset;

		if (PrevBlockIt != m_FreeBlocksByOffset.end() && Offset == PrevBlockIt->first + PrevBlockIt->second.Size)
		{
			// PrevBlock.Offset           Offset
			// |                          |
			// |<-----PrevBlock.Size----->|<------Size-------->|
			//
			NewSize = PrevBlockIt->second.Size + Size;
			NewOffset = PrevBlockIt->first;

			if (NextBlockIt != m_FreeBlocksByOffset.end() && Offset + Size == NextBlockIt->first)
			{
				// PrevBlock.Offset           Offset               NextBlock.Offset 
				// |                          |                    |
				// |<-----PrevBlock.Size----->|<------Size-------->|<-----NextBlock.Size----->|
				//
				NewSize += NextBlockIt->first;
				m_FreeBlocksBySize.erase(PrevBlockIt->second.OrderBySizeIt);
				m_FreeBlocksBySize.erase(NextBlockIt->second.OrderBySizeIt);
				++NextBlockIt;
				m_FreeBlocksByOffset.erase(PrevBlockIt, NextBlockIt);
			}
			else
			{
				//   PrevBlock.Offset           Offset                     NextBlock.Offset
				//     |                          |                             |
				//     |<-----PrevBlock.Size----->|<------Size-------->| ~ ~ ~  |<-----NextBlock.Size----->|
				//
				m_FreeBlocksBySize.erase(PrevBlockIt->second.OrderBySizeIt);
				m_FreeBlocksByOffset.erase(PrevBlockIt);
			}
		}
		else if (PrevBlockIt != m_FreeBlocksByOffset.end() && Offset + Size == NextBlockIt->first)
		{
			//   PrevBlock.Offset                      Offset              NextBlock.Offset
			//     |                                  |                    |
			//     |<-----PrevBlock.Size-----> ~ ~ ~  |<------Size-------->|<-----NextBlock.Size----->|
			//
			NewSize = Size + NextBlockIt->second.Size;
			NewOffset = Offset;
			m_FreeBlocksBySize.erase(NextBlockIt->second.OrderBySizeIt);
			m_FreeBlocksByOffset.erase(NextBlockIt);
		}
		else
		{
			// PrevBlock.Offset                   Offset                       NextBlock.Offset 
			// |                                  |                            |
			// |<-----PrevBlock.Size----->| ~ ~ ~ |<------Size-------->| ~ ~ ~ |<-----NextBlock.Size----->|
			//
			NewOffset = Offset;
			NewSize = Size;
		}

		AddNewBlock(NewOffset,NewSize);
		m_FreeSize += Size;
	}

	void FreeListAllocator::Free(Allocation& allocation)
	{
		Free(allocation.Offset, allocation.Size);
		allocation = Allocation::InvalidAllocation();
	}


	FreeListGPUAllocator::FreeListGPUAllocator(OffsetType MaxSize)
		: FreeListAllocator(MaxSize)
	{

	}

	FreeListGPUAllocator::~FreeListGPUAllocator()
	{
		if (!m_StaleAllocations.empty() && m_StaleAllocationsSize != 0)
		{
			rsim_warn("FreeListGPUAllocator::~FreeListGPUAllocator: Some stale allocations were not released.");
		}
	}

	void FreeListGPUAllocator::Free(OffsetType Offset, OffsetType Size, uint64_t FenceValue)
	{
		m_StaleAllocations.emplace_back(Offset, Size, FenceValue);
		m_StaleAllocationsSize += Size;
	}

	void FreeListGPUAllocator::Free(FreeListAllocator::Allocation& allocation, uint64_t FenceValue)
	{
		Free(allocation.Offset, allocation.Size, FenceValue);
		allocation = Allocation::InvalidAllocation();
	}

	FreeListGPUAllocator::FreeListGPUAllocator(FreeListGPUAllocator&& rhs) noexcept
		:
		FreeListAllocator(std::move(rhs)),
		m_StaleAllocations(std::move(rhs.m_StaleAllocations)),
		m_StaleAllocationsSize(rhs.m_StaleAllocationsSize)
	{
	}

	void FreeListGPUAllocator::ReleaseStaleAllocations(uint64_t LastCompletedFenceValue)
	{
		while (!m_StaleAllocations.empty() && m_StaleAllocations.front().FenceValue <= LastCompletedFenceValue)
		{
			auto const& OldestAllocation = m_StaleAllocations.front();
			FreeListAllocator::Free(OldestAllocation.Offset, OldestAllocation.Size);
			m_StaleAllocationsSize -= OldestAllocation.Size;
			m_StaleAllocations.pop_front();
		}
	}
}
