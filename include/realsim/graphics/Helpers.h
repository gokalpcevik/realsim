#pragma once
#include <d3d12.h>

namespace RSim::Graphics
{
	struct DescriptorHandleIncrementSize
	{
		UINT RTVIncrementSize;
		UINT DSVIncrementSize;
		UINT CBV_SRV_UAVIncrementSize;
		UINT SamplerSize;
	};

	/**
	 * \brief Returns the descriptor handle increment sizes, as a struct.
	 * \param pDevice The device pointer.
	 */
	DescriptorHandleIncrementSize GetDescriptorHandleIncrementSizes(ID3D12Device* pDevice);
}