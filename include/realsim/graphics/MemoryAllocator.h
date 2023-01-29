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
	 * \param device The GraphicsDevice instance.
	 * \return The newly created memory allocator.
	 */
	std::unique_ptr<MemoryAllocator> CreateMemoryAllocator(GraphicsDevice const& device);

	/**
	 * \brief Creates a memory allocator for the renderer with the given allocator description options.
	 * \param device The GraphicsDevice instance.
	 * \param allocatorDesc Allocator description that will be used to construct the allocator.
	 */
	std::unique_ptr<MemoryAllocator> CreateMemoryAllocator(GraphicsDevice const& device, D3D12MA::ALLOCATOR_DESC const& allocatorDesc);

	class MemoryAllocator final
	{
	public:
		MemoryAllocator& operator=(MemoryAllocator const&) = delete;
		MemoryAllocator(MemoryAllocator const&) = delete;

		~MemoryAllocator();

		[[nodiscard]] std::unique_ptr<MemoryAllocation> CreateResource(D3D12MA::ALLOCATION_DESC const* pAllocDesc,
		                                                               D3D12_RESOURCE_DESC const* pResourceDesc, 
		                                                               D3D12_RESOURCE_STATES InitialResourceState, 
		                                                               D3D12_CLEAR_VALUE const* pOptimizedClearValue,
		                                                               ID3D12Resource* pResource = nullptr) const;

		/**
		 * \brief Logs the information about the current budget.
		 */
		void LogBudget() const;
		
		/**
		 * \brief 'Gets' the budget, does not calculate and should be fast enough to call every frame.
		 * \return Returns the budget of the GPU and the CPU, respectively in the pair order.
		 */
		[[nodiscard]] inline std::pair<D3D12MA::Budget, D3D12MA::Budget> GetBudget() const;

	private:
		MemoryAllocator(GraphicsDevice const& device);
		MemoryAllocator(GraphicsDevice const& device,D3D12MA::ALLOCATOR_DESC const& allocatorDesc);

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
