#pragma once

#include <memory>
#include <D3D12MemAlloc.h>
#include <wrl.h>

#include "realsim/graphics/MemoryAllocation.h"
#include "realsim/graphics/GraphicsDevice.h"
#include "realsim/graphics/Exception.h"
#include "realsim/core/Logger.h"
#include "realsim/core/Assert.h"

namespace RSim::Graphics
{
	class MemoryAllocator;

	/**
	 * \brief Creates a memory allocator for the renderer with the default allocator description options.
	 * \param device A GraphicsDevice instance.
	 * \return The newly created memory allocator.
	 */
	std::unique_ptr<MemoryAllocator> CreateMemoryAllocator(GraphicsDevice const& device);
	std::unique_ptr<MemoryAllocator> CreateMemoryAllocator(GraphicsDevice const& device, D3D12MA::ALLOCATOR_DESC const& allocatorDesc);

	class MemoryAllocator final
	{
	public:
		~MemoryAllocator();

		[[nodiscard]] std::unique_ptr<MemoryAllocation> CreateResource(D3D12MA::ALLOCATION_DESC const* pAllocDesc,
			D3D12_RESOURCE_DESC const* pResourceDesc, 
			D3D12_RESOURCE_STATES InitialResourceState, 
			D3D12_CLEAR_VALUE const* pOptimizedClearValue,
			ID3D12Resource* pResource = nullptr) const;

		void LogBudget() const;

	private:
		MemoryAllocator(GraphicsDevice const& device);
		MemoryAllocator(GraphicsDevice const& device,D3D12MA::ALLOCATOR_DESC const& allocatorDesc);
		MemoryAllocator(MemoryAllocator const&) = delete;
		MemoryAllocator& operator=(MemoryAllocator const&) = delete;

		friend std::unique_ptr<MemoryAllocator> CreateMemoryAllocator(GraphicsDevice const& device)
		{
			return std::unique_ptr<MemoryAllocator>(new MemoryAllocator(device));
		}

		friend std::unique_ptr<MemoryAllocator> CreateMemoryAllocator(GraphicsDevice const& device, D3D12MA::ALLOCATOR_DESC const& allocatorDesc)
		{
			return std::unique_ptr<MemoryAllocator>(new MemoryAllocator(device, allocatorDesc));
		}
	private:
		D3D12MA::Allocator* m_Allocator{ nullptr };
	};
}