//
// Created by GOKALP on 3/20/2023.
//

#include "AssetCache.h"

namespace RSim::Graphics {
    using namespace AssetLib;

    AssetHandle GetHandleFromFile(std::filesystem::path const& path) {
        return std::filesystem::hash_value(path);
    }

    AssetCache::AssetCache() {
        // does this work
        m_AssetCache.set_deleted_key(GetHandleFromFile("REALSIM_ASSET_CACHE_DELETED_KEY"));
        m_AssetCache.set_empty_key(GetHandleFromFile("REALSIM_ASSET_CACHE_EMPTY_KEY"));
    }

    bool AssetCache::IsValid(AssetHandle handle) {
        return m_AssetCache.find(handle) != m_AssetCache.end();
    }

    AssetRef AssetCache::Get(AssetHandle handle) {
        return m_AssetCache[handle];
    }

    void AssetCache::Delete(AssetHandle handle) {
        m_AssetCache.erase(handle);
    }

    AssetRef AssetCache::Load(fs::path const& path) {
        AssetRef loaded = AssetLib::LoadBinaryFileRef(path);
        Insert(loaded);
        return loaded;
    }

    void AssetCache::Insert(AssetRef const& asset) {
        m_AssetCache.insert({asset->HashValue,asset});
    }
}