#include "realsim/graphics/Helpers.h"

namespace RSim::Graphics
{
	DescriptorHandleIncrementSize GetDescriptorHandleIncrementSizes(ID3D12Device* pDevice)
	{
		UINT rtv = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		UINT dsv = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		UINT uav_srv_cbv = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		UINT sampler = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		return { rtv,dsv,uav_srv_cbv,sampler };
	}
}
