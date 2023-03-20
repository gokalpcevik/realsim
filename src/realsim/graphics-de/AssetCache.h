#pragma once
#include "assetlib/AssetLoader.h"
#include "assetlib/MeshLoader.h"
#include "google/dense_hash_map"

namespace RSim::Graphics
{
    namespace fs = std::filesystem;

    AssetLib::AssetHandle GetHandleFromFile(fs::path const& path);

   class AssetCache
   {
   public:
       AssetCache();

       bool IsValid(AssetLib::AssetHandle handle);
       AssetLib::AssetRef Get(AssetLib::AssetHandle handle);
       AssetLib::AssetRef Load(fs::path const& path);
       void Insert(AssetLib::AssetRef const& asset);
       void Delete(AssetLib::AssetHandle handle);
   private:
        google::dense_hash_map<AssetLib::AssetHandle, AssetLib::AssetRef> m_AssetCache;
   };
}

