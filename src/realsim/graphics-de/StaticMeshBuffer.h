#pragma once
#include "RefCntAutoPtr.hpp"
#include "Buffer.h"
#include "RenderDevice.h"

#include "assetlib/AssetLoader.h"
#include "assetlib/MeshLoader.h"

namespace RSim::Graphics
{
	namespace Dl = Diligent;

	Dl::RefCntAutoPtr<Dl::IBuffer> CreateVertexBufferFromMeshAsset(Dl::IRenderDevice* pDevice, 
		AssetLib::MeshInfo* pMeshInfo, 
		AssetLib::UnpackedMesh* pUnpacked,
		std::string const& BufferName = "Mesh Vertex Buffer");

	Dl::RefCntAutoPtr<Dl::IBuffer> CreateIndexBufferFromMeshAsset(Dl::IRenderDevice* pDevice, 
		AssetLib::MeshInfo* pMeshInfo, 
		AssetLib::UnpackedMesh* pUnpacked,
		std::string const& BufferName = "Mesh Index Buffer");
}