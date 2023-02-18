#include "realsim/graphics/CommandAllocator.h"

namespace RSim::Graphics
{
	CommandAllocatorPool::CommandAllocatorPool(ID3D12Device2* pDevice, D3D12_COMMAND_LIST_TYPE CommandType)
		: m_CommandType(CommandType), m_Device(pDevice)
	{
	}

	CommandAllocatorPool::CommandAllocatorPool(CommandAllocatorPool&& rhs) noexcept
	{
		m_CommandType = rhs.m_CommandType;
		m_Device = rhs.m_Device;
		m_AllocatorPool = std::move(rhs.m_AllocatorPool);
		m_ReadyAllocators = std::move(rhs.m_ReadyAllocators);
	}

	CommandAllocatorPool& CommandAllocatorPool::operator=(CommandAllocatorPool&& rhs) noexcept
	{
		m_CommandType = rhs.m_CommandType;
		m_Device = rhs.m_Device;
		m_AllocatorPool = std::move(rhs.m_AllocatorPool);
		m_ReadyAllocators = std::move(rhs.m_ReadyAllocators);
		return *this;
	}

	CommandAllocatorPool::~CommandAllocatorPool()
	{
		for (auto& cmdAllocator : m_AllocatorPool)
		{
			cmdAllocator.Reset();
		}
		m_AllocatorPool.clear();
	}

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocatorPool::RequestCommandAllocator(uint64_t CompletedFenceValue)
	{
		std::lock_guard lockGuard(m_AllocatorMutex);

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pAllocator{ nullptr };

		// Check if there are ready(previously discarded) allocators ready
		if (!m_ReadyAllocators.empty())
		{
			std::pair<uint64_t, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> const& AllocatorPair = m_ReadyAllocators.front();

			// If the allocators fence value is smaller than the completed fence value(frame number), it means the allocator is now ready to use.
			if (AllocatorPair.first <= CompletedFenceValue)
			{
				pAllocator = AllocatorPair.second;
				ThrowIfFailed(pAllocator->Reset());
				m_ReadyAllocators.pop();
			}
		}
		// If there aren't ready allocators to be used
		if(pAllocator == nullptr)
		{
			ThrowIfFailed(m_Device->CreateCommandAllocator(m_CommandType, IID_PPV_ARGS(&pAllocator)));
			pAllocator->SetName(fmt::format(L"Command Allocator {0}", m_AllocatorPool.size()).c_str());
			m_AllocatorPool.push_back(pAllocator);
		}

		return pAllocator;
	}

	void CommandAllocatorPool::DiscardAllocator(uint64_t FenceValue, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> const& CommandAllocator)
	{
		std::lock_guard lockGuard(m_AllocatorMutex);

		m_ReadyAllocators.push(std::make_pair(FenceValue,CommandAllocator));
	}
}
