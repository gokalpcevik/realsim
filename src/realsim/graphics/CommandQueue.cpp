#include "realsim/graphics/CommandQueue.h"

namespace RSim::Graphics
{
	CommandQueue::CommandQueue(ID3D12Device2* pDevice, D3D12_COMMAND_LIST_TYPE CommandType)
	{
		m_CmdType = CommandType;
		m_NextFenceValue = ((uint64_t)m_CmdType << 56) + 1;
		m_LastCompletedFenceValue = ((uint64_t)m_CmdType << 56);

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = m_CmdType;
		queueDesc.NodeMask = 0;
		ThrowIfFailed(pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CmdQueue)));

		m_CmdQueue->SetName(L"CommandQueue");

		ThrowIfFailed(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

		m_Fence->SetName(L"CommandQueue::m_Fence");

		m_Fence->Signal(m_LastCompletedFenceValue);

		m_hFenceEvent = CreateEventEx(NULL, false, false, EVENT_ALL_ACCESS);
		assert(m_hFenceEvent != INVALID_HANDLE_VALUE);

		m_AllocatorPool = std::make_unique<CommandAllocatorPool>(pDevice, CommandType);
		assert(m_CmdQueue);
	}

	CommandQueue::~CommandQueue()
	{
		if (m_CmdQueue == nullptr) return;
		CloseHandle(m_hFenceEvent);
	}

	uint64_t CommandQueue::PollCurrentFenceValue()
	{
		m_LastCompletedFenceValue = std::max(m_LastCompletedFenceValue, m_Fence->GetCompletedValue());
		return m_LastCompletedFenceValue;
	}

	bool CommandQueue::IsFenceCompleted(uint64_t FenceValue)
	{
		if (FenceValue > m_LastCompletedFenceValue)
		{
			PollCurrentFenceValue();
		}

		return FenceValue <= m_LastCompletedFenceValue;
	}

	void CommandQueue::InsertWait(uint64_t FenceValue) const
	{
		m_CmdQueue->Wait(m_Fence.Get(), FenceValue);
	}

	void CommandQueue::InsertWaitForQueueFence(CommandQueue const& Other, uint64_t FenceValue) const
	{
		m_CmdQueue->Wait(Other.GetFence().Get(), FenceValue);
	}

	void CommandQueue::InsertWaitForQueue(CommandQueue const& Other) const
	{
		m_CmdQueue->Wait(Other.GetFence().Get(), Other.GetNextFenceValue() - 1);
	}

	void CommandQueue::WaitForFence(uint64_t FenceValue)
	{
		if (IsFenceCompleted(FenceValue))
		{
			return;
		}
		{
			std::lock_guard lock(m_EventMutex);
			m_Fence->SetEventOnCompletion(FenceValue, m_hFenceEvent);

			::WaitForSingleObjectEx(m_hFenceEvent, INFINITE, false);
			m_LastCompletedFenceValue = FenceValue;
		}
	}

	void CommandQueue::WaitForIdle()
	{
		WaitForFence(IncrementFence());
	}

	uint64_t CommandQueue::ExecuteCommandList(ID3D12CommandList* pCommandList)
	{
		std::lock_guard lock(m_FenceMutex);

		ThrowIfFailed(static_cast<ID3D12GraphicsCommandList1*>(pCommandList)->Close());

		m_CmdQueue->ExecuteCommandLists(1, &pCommandList);

		m_CmdQueue->Signal(m_Fence.Get(), m_NextFenceValue);

		return m_NextFenceValue++;
	}

	uint64_t CommandQueue::IncrementFence()
	{
		std::lock_guard lock(m_FenceMutex);

		m_CmdQueue->Signal(m_Fence.Get(), m_NextFenceValue);
		return m_NextFenceValue++;
	}

	CommandListController::CommandListController(ID3D12Device2* pDevice)
		: m_pDevice(pDevice)
	{
		m_GraphicsQueue = std::make_unique<CommandQueue>(pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_ComputeQueue = std::make_unique<CommandQueue>(pDevice, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		m_CopyQueue = std::make_unique<CommandQueue>(pDevice, D3D12_COMMAND_LIST_TYPE_COPY);
	}

	CommandQueue& CommandListController::GetQueue(D3D12_COMMAND_LIST_TYPE CommandType) const
	{
		switch (CommandType)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT: return GetGraphicsQueue();
		case D3D12_COMMAND_LIST_TYPE_BUNDLE: throw std::exception("Bundles are not supported yet.");
		case D3D12_COMMAND_LIST_TYPE_COMPUTE: return GetComputeQueue();
		case D3D12_COMMAND_LIST_TYPE_COPY: return GetCopyQueue();
		default: throw std::exception();
		}
	}

	bool CommandListController::IsFenceComplete(uint64_t FenceValue) const
	{
		return GetQueue(static_cast<D3D12_COMMAND_LIST_TYPE>(FenceValue >> 56)).IsFenceCompleted(FenceValue);
	}

	void CommandListController::WaitForFence(uint64_t FenceValue) const
	{
		CommandQueue& queue = GetQueue(static_cast<D3D12_COMMAND_LIST_TYPE>(FenceValue >> 56));
		queue.WaitForFence(FenceValue);
	}

	void CommandListController::Flush() const
	{
		m_GraphicsQueue->WaitForIdle();
		m_ComputeQueue->WaitForIdle();
		m_CopyQueue->WaitForIdle();
	}
}
