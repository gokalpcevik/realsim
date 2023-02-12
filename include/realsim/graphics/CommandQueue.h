
#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <queue>
#include <mutex>
#include <cstdint>

#include "realsim/graphics/CommandAllocator.h"
#include "realsim/graphics/Exception.h"
#include "realsim/Core/Assert.h"

namespace RSim::Graphics
{
	/**
	 * \brief A class that can be used to manage the command queues and synchronization cleanly. Largely inspired from MiniEngine from Microsoft DirectX-Graphics-Samples
	 * and https://alextardif.com/D3D11To12P1.html.
	 */
	class CommandQueue
	{
	public:
		CommandQueue(ID3D12Device2* pDevice, D3D12_COMMAND_LIST_TYPE CommandType);
		CommandQueue(CommandQueue const&) = delete;
		CommandQueue& operator=(CommandQueue const&) = delete;

		~CommandQueue();

		/**
		 * \brief Polls the current fence value and returns it. Internally, it sets m_LastCompletedFenceValue(can be retrieved from CommandQueue::GetLastCompletedFenceValue())
		 * to fence's completed value if the fence's completed value is larger than the last completed fence value.
		 * \return Returns the current fence value.
		*/
		uint64_t PollCurrentFenceValue();


		/**
		 * \brief Checks whether a fence is completed.
		 * \param FenceValue Fence value to check if completed.
		 */
		bool IsFenceCompleted(uint64_t FenceValue);

		void InsertWait(uint64_t FenceValue) const;
		void InsertWaitForQueueFence(CommandQueue const& Other,uint64_t FenceValue) const;
		void InsertWaitForQueue(CommandQueue const& Other) const;
		void WaitForFence(uint64_t FenceValue);
		void WaitForIdle();

		/**
		 * \brief Executes a given command list. It closes the command list and then calls ID3D12CommandQueue::ExecuteCommandLists. 
		 * \return Returns the signaled fence value.
		 */
		uint64_t ExecuteCommandList(ID3D12CommandList* pCommandList);

		[[nodiscard]] inline Microsoft::WRL::ComPtr<ID3D12CommandQueue> const& GetCommandQueue() const { return m_CmdQueue; }

		[[nodiscard]] inline uint64_t GetLastCompletedFenceValue() const { return m_LastCompletedFenceValue; }
		[[nodiscard]] inline uint64_t GetNextFenceValue() const { return m_NextFenceValue; }
		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Fence> const& GetFence() const { return m_Fence; }
	private:
		uint64_t IncrementFence();
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CmdQueue{nullptr};
		D3D12_COMMAND_LIST_TYPE m_CmdType{};

		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence{nullptr};
		uint64_t m_LastCompletedFenceValue{};
		uint64_t m_NextFenceValue{};
		HANDLE m_hFenceEvent{};

		std::mutex m_EventMutex;
		std::mutex m_FenceMutex;

		std::unique_ptr<CommandAllocatorPool> m_AllocatorPool;
	};


	/**
	 * \brief A wrapper class around CommandQueue for ease of use with different types of command lists. It contains three types of command queues: direct, compute and copy. They can
	 * be retrieved by calling CommandListController::GetQueue(type).
	 */
	class CommandListController
	{
	public:
		explicit CommandListController(ID3D12Device2* pDevice);
		CommandListController(CommandListController const&) = delete;
		CommandListController& operator=(CommandListController const&) = delete;
		~CommandListController() = default;

		[[nodiscard]] CommandQueue& GetGraphicsQueue() const { return *m_GraphicsQueue; }
		[[nodiscard]] CommandQueue& GetComputeQueue() const { return *m_ComputeQueue;  }
		[[nodiscard]] CommandQueue& GetCopyQueue() const { return *m_CopyQueue; }

		[[nodiscard]] CommandQueue& GetQueue(D3D12_COMMAND_LIST_TYPE CommandType) const;

		/**
		 * \brief Check if the fence is completed. By using the logical shift-left trick using the types, the command list and queue type is automagically deduced.
		 */
		[[nodiscard]] bool IsFenceComplete(uint64_t FenceValue) const;

		/**
		 * \brief The CPU will wait for a fence to reach a specified value. By using the logical shift-left trick using the types, the command list and queue type is automagically deduced.
		 */
		void WaitForFence(uint64_t FenceValue) const;

		/**
		 * \brief The CPU will wait for all command queues to empty (so that the GPU is idle)
		 */
		void Flush() const;

	private:
		ID3D12Device* m_pDevice{ nullptr };

		std::unique_ptr<CommandQueue> m_GraphicsQueue;
		std::unique_ptr<CommandQueue> m_ComputeQueue;
		std::unique_ptr<CommandQueue> m_CopyQueue;
	};
}