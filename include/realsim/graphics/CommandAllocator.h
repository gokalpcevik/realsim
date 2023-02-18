
#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <queue>
#include <cstdint>
#include <mutex>
#include <fmt/xchar.h>

#include "realsim/graphics/Exception.h"

namespace RSim::Graphics
{
	/**
	 * \brief This class provides 'on-demand' command allocators, that are pooled.
	 * Mostly inspired by the Microsoft MiniEngine from the Microsoft's DirectX-Graphics-Samples.
	 */
	class CommandAllocatorPool
	{
	public:
		CommandAllocatorPool(ID3D12Device2* pDevice, D3D12_COMMAND_LIST_TYPE CommandType);
		CommandAllocatorPool(CommandAllocatorPool const&) = delete;
		CommandAllocatorPool& operator=(CommandAllocatorPool const&) = delete;
		CommandAllocatorPool(CommandAllocatorPool&& rhs) noexcept;
		CommandAllocatorPool& operator=(CommandAllocatorPool&& rhs) noexcept;

		~CommandAllocatorPool();

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> RequestCommandAllocator(uint64_t CompletedFenceValue);

		void DiscardAllocator(uint64_t FenceValue, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> const& CommandAllocator);

		[[nodiscard]] inline size_t Size() const { return m_AllocatorPool.size(); }
	private:
		D3D12_COMMAND_LIST_TYPE m_CommandType;

		ID3D12Device2* m_Device{nullptr};
		std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> m_AllocatorPool;
		std::queue<std::pair<uint64_t, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>> m_ReadyAllocators;
		std::mutex m_AllocatorMutex;
	};

}