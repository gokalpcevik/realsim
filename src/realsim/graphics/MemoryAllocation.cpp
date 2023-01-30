#include "realsim/graphics/MemoryAllocation.h"

namespace RSim::Graphics
{
	MemoryAllocation::MemoryAllocation(D3D12MA::Allocation* pAllocation)
		: m_Allocation(pAllocation)
	{

	}

	MemoryAllocation::~MemoryAllocation()
	{
		if(m_Allocation)
		{
			m_Allocation->Release();
		}
	}

	ID3D12Resource* MemoryAllocation::GetResource() const noexcept
	{
		return m_Allocation->GetResource();
	}

	ID3D12Heap* MemoryAllocation::GetHeap() const noexcept
	{
		return m_Allocation->GetHeap();
	}

	BOOL MemoryAllocation::WasZeroInitialized() const noexcept
	{
		return m_Allocation->WasZeroInitialized();
	}

	UINT64 MemoryAllocation::GetOffset() const noexcept
	{
		return m_Allocation->GetOffset();
	}

	UINT64 MemoryAllocation::GetSize() const noexcept
	{
		return m_Allocation->GetSize();
	}

	LPCWSTR MemoryAllocation::GetName() const noexcept
	{
		return m_Allocation->GetName();
	}

	void MemoryAllocation::SetName(LPCWSTR name) const noexcept
	{
		m_Allocation->SetName(name);
	}
}
