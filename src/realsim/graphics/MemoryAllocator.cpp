#include "realsim/graphics/MemoryAllocator.h"

namespace RSim::Graphics
{
	MemoryAllocator::MemoryAllocator(GraphicsDevice const& device)
	{
		D3D12MA::ALLOCATOR_DESC allocatorDesc{};
		
		allocatorDesc.Flags = D3D12MA::ALLOCATOR_FLAG_NONE;
		allocatorDesc.PreferredBlockSize = { 0 };
		allocatorDesc.pAllocationCallbacks = nullptr;
		allocatorDesc.pAdapter = device.GetAdapter1Raw();
		allocatorDesc.pDevice = device.GetDevice2Raw();

		HRESULT hr = D3D12MA::CreateAllocator(&allocatorDesc, &m_Allocator);

		if(FAILED(hr))
		{
			rsim_error("D3D12MemoryAllocator cannot be created.");
			RSIM_ASSERTM(false, "D3D12MemoryAllocator cannot be created.");
		}
		rsim_info("Memory allocator has been created with the following budget:");
		this->LogBudget();
	}

	MemoryAllocator::MemoryAllocator(GraphicsDevice const& device, D3D12MA::ALLOCATOR_DESC const& allocatorDesc)
	{
		HRESULT hr = D3D12MA::CreateAllocator(&allocatorDesc, &m_Allocator);

		if (FAILED(hr))
		{
			rsim_error("D3D12MemoryAllocator cannot be created.");
			RSIM_ASSERTM(false, "D3D12MemoryAllocator cannot be created.");
		}

		LogBudget();
	}

	MemoryAllocator::~MemoryAllocator()
	{
		if(m_Allocator)
			m_Allocator->Release();
	}

	std::unique_ptr<MemoryAllocation> MemoryAllocator::CreateResource(D3D12MA::ALLOCATION_DESC const* pAllocDesc,
		D3D12_RESOURCE_DESC const* pResourceDesc, 
		D3D12_RESOURCE_STATES InitialResourceState,
		D3D12_CLEAR_VALUE const* pOptimizedClearValue,
		ID3D12Resource* pResource) const
	{
		D3D12MA::Allocation* allocation{nullptr};

		HRESULT hr = m_Allocator->CreateResource(pAllocDesc, pResourceDesc, 
			InitialResourceState, 
			pOptimizedClearValue, 
			&allocation, 
			__uuidof(ID3D12Resource),
		    reinterpret_cast<void**>(&pResource));

		ThrowIfFailed(hr);
		return std::unique_ptr<MemoryAllocation>(new MemoryAllocation(allocation));
	}

	void MemoryAllocator::LogBudget() const
	{
		D3D12MA::Budget GPUBudget{}, CPUBudget{};

		m_Allocator->GetBudget(&GPUBudget, &CPUBudget);
		rsim_trace("Video Memory Budget:{} MB, CPU Memory Budget:{} MB",
			GPUBudget.BudgetBytes / 1024 / 1024,
			CPUBudget.BudgetBytes / 1024 / 1024);
	}

	std::pair<D3D12MA::Budget, D3D12MA::Budget> MemoryAllocator::GetBudget() const
	{
		D3D12MA::Budget GPUBudget{}, CPUBudget{};
		m_Allocator->GetBudget(&GPUBudget, &CPUBudget);
		return {GPUBudget,CPUBudget};
	}
}
