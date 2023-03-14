#include "realsim/graphics-de/StaticMeshBuffer.h"

namespace RSim::Graphics
{
	using namespace Diligent;

	RefCntAutoPtr<IBuffer> CreateVertexBufferFromMeshAsset(IRenderDevice* pDevice,
		AssetLib::MeshInfo* pMeshInfo,
		AssetLib::UnpackedMesh* pUnpacked,
		std::string const& BufferName)
	{
		BufferDesc VertBuffDesc;
		VertBuffDesc.Name = BufferName.c_str();
		VertBuffDesc.Usage = USAGE_IMMUTABLE;
		VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
		VertBuffDesc.Size = pMeshInfo->VertexBufferSizeInBytes;
		BufferData VBData;
		VBData.pData = pUnpacked->VertexBufferBlob.data();
		VBData.DataSize = pMeshInfo->VertexBufferSizeInBytes;

		RefCntAutoPtr<IBuffer> vertexBuffer;
		pDevice->CreateBuffer(VertBuffDesc, &VBData, &vertexBuffer);
		return vertexBuffer;
	}

	RefCntAutoPtr<IBuffer> CreateIndexBufferFromMeshAsset(IRenderDevice* pDevice, 
		AssetLib::MeshInfo* pMeshInfo, 
		AssetLib::UnpackedMesh* pUnpacked,
		std::string const& BufferName)
	{
		BufferDesc IndBuffDesc;
		IndBuffDesc.Name = BufferName.c_str();
		IndBuffDesc.Usage = USAGE_IMMUTABLE;
		IndBuffDesc.BindFlags = BIND_INDEX_BUFFER;
		IndBuffDesc.Size = pMeshInfo->IndexBufferSizeInBytes;
		BufferData IBData;
		IBData.pData = pUnpacked->IndexBufferBlob.data();
		IBData.DataSize = pMeshInfo->IndexBufferSizeInBytes;

		RefCntAutoPtr<IBuffer> indexBuffer;
		pDevice->CreateBuffer(IndBuffDesc, &IBData, &indexBuffer);
		return indexBuffer;
	}
}
