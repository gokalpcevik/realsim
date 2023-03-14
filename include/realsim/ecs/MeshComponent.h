#pragma once
#include <memory>

#include "realsim/realsim.h"
#include "realsim/graphics-de/Drawable.h"

#include "assetlib/AssetLoader.h"
#include "assetlib/MeshLoader.h"

namespace RSim::ECS
{
	struct MaterialConstants
	{
		Diligent::float4 Color{ 1.0f,1.0f,1.0f,1.0f };
		float Shininess = 1.0f;
	};

	struct MeshComponent
	{
		AssetLib::MeshInfo* pMeshInfo;
		AssetLib::Asset* pAsset;
		Graphics::Drawable* Drawable;
		MaterialConstants Material{};
	};
}