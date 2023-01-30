#pragma once
#define NOMINMAX
#include <d3d12.h>
#include <wrl.h>
#include <array>
#include <d3dx12.h>

#include "realsim/core/Window.h"
#include "realsim/core/Logger.h"

#include "realsim/graphics/GraphicsDevice.h"
#include "realsim/graphics/SwapChain.h"
#include "realsim/graphics/MemoryAllocator.h"
#include "realsim/graphics/MemoryAllocation.h"
#include "realsim/graphics/RendererConfiguration.h"
#include "realsim/graphics/CommandAllocator.h"
#include "realsim/graphics/CommandQueue.h"
#include "realsim/graphics/Exception.h"
#include "realsim/graphics/Helpers.h"


namespace RSim::Graphics
{
	class Renderer
	{
	public:
		Renderer(Core::Window const* outputWindow);

		void Clear(FLOAT const color[]) const;
		void Present();
		void Resize(uint32_t width, uint32_t height);
	private:
		void InitVariables();

	private:
		void ClearDepthStencilView(D3D12_CLEAR_FLAGS flags, FLOAT depth) const;
		void ClearRenderTargetView(FLOAT const color[]) const;

		void ResizeDepthBuffer(uint32_t width, uint32_t height);

		//----------------------------------Convenience Functions-----------------------------------------------
		// These names should be fine as they are only used within the class itself anyway(private).			
		/**																										
		 * \brief Convenience function for faster typing, instead of m_MemAllocator.							
		 */																										
		[[nodiscard]] inline auto const& MemAllocator() const { return m_MemAllocator; }									
		/**
		 * \brief Convenience function for faster typing, gets the graphics queue from the command list controller.
		 */
		[[nodiscard]] inline CommandQueue& GfxQueue() const { return m_CmdListController->GetGraphicsQueue(); }
		/**
		 * \brief Convenience function for faster typing, gets the graphics queue from the command list controller.
		 */
		[[nodiscard]] inline CommandQueue& ComputeQueue() const { return m_CmdListController->GetComputeQueue(); }
		/**
		 * \brief Convenience function for faster typing, gets the graphics queue from the command list controller.
		 */
		[[nodiscard]] inline CommandQueue& CopyQueue() const { return m_CmdListController->GetCopyQueue(); }
		/**																										
		 * \brief Convenience function for faster typing, instead of m_GfxDevice.								
		 */																										
		[[nodiscard]] inline auto const& Device() const { return m_GfxDevice; }										
		/**																										
		 * \brief Convenience function for faster typing, instead of m_SwapChain.								
		 */																										
		[[nodiscard]] inline auto const& SwpChain() const { return m_SwapChain; }										
		//------------------------------------------------------------------------------------------------------

	private:
		Core::Window const* m_pOutputWindow{nullptr};

		std::unique_ptr<GraphicsDevice> m_GfxDevice;
		std::unique_ptr<SwapChain> m_SwapChain;
		std::unique_ptr<MemoryAllocator> m_MemAllocator;

		std::unique_ptr<CommandAllocatorPool> m_CmdAllocatorPool;
		std::unique_ptr<CommandListController> m_CmdListController;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CmdQueue;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> m_CmdList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CmdAllocators[NumFramesInFlight];
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVDescriptorHeap;
		UINT m_CurrentBackBufferIndex{0UL};
		std::array<ID3D12Resource*, NumFramesInFlight> m_BackBuffers{nullptr};
		std::unique_ptr<MemoryAllocation> m_DepthBuffer;

		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
		HANDLE m_FenceEvent{INVALID_HANDLE_VALUE};
		std::uint64_t m_FrameNumber{0ULL};
		std::array<std::uint64_t, NumFramesInFlight> m_FrameFenceValues{};

		DescriptorHandleIncrementSize m_DescriptorHandleIncrementSizes{};

		D3D12_VIEWPORT m_Viewport{ CD3DX12_VIEWPORT(0.0f,0.0f,1366,768) };
		D3D12_RECT m_ScissorRect{ CD3DX12_RECT{0,0,LONG_MAX,LONG_MAX} };
	};
}