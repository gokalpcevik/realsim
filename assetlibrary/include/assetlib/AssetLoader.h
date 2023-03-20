#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include "assetlib/FileHelper.h"

namespace RSim::AssetLib
{
	enum class CompressionMode
	{
		None = 0,
		LZ4
	};

	using AssetHandle = uint64_t;

	struct Asset
	{
		Asset() = default;
		Asset(Asset const&) = default;
		Asset& operator=(Asset const&) = default;
		~Asset() = default;

		Asset(Asset&& rhs) noexcept
		{
			std::memcpy(Type,rhs.Type,4);
			Metadata = std::move(rhs.Metadata);
			BinaryBlob = std::move(rhs.BinaryBlob);
		}

		Asset& operator=(Asset&& rhs) noexcept
		{
			std::memcpy(Type, rhs.Type, 4);
			Version = rhs.Version;
			Metadata = std::move(rhs.Metadata);
			BinaryBlob = std::move(rhs.BinaryBlob);
			return *this;
		}

		bool operator==(Asset const& rhs) const
		{
			return Version == rhs.Version && std::memcmp(Type,rhs.Type,sizeof(Type)) == 0;
		}

		/**
		 * \brief Type of the asset: \n
		 * 'TEXI' for textures \n
		 * 'MESH' for meshes \n
		 * 'MATL' for materials
		 * 'NULL' if the asset is invalid/null
		 */
		char Type[4] {'N','U','L','L'};
		/**
		 * \brief Version
		 */
		int Version { std::numeric_limits<int>::max() };
		/**
		 * \brief Header json that contains metadata about the object.
		 */
		std::string Metadata {};
		/**
		 * \brief Raw data of the object, compressed , like the pixels of a texture or vertices of a mesh file.
		 */
		std::vector<char> BinaryBlob{};

		/**
		 * \brief Total size of the compressed asset(lying on the disk), including the binary blob, metadata and other fields.
		 * - This does not get saved into the asset file, it is only set when loading a binary file.
		 */
		size_t TotalCompressedSizeInBytes;

		uint64_t HashValue;

        static Asset const Null;
    };

    using AssetRef = std::shared_ptr<Asset>;

	bool SaveBinaryFile(std::filesystem::path const& Path, Asset const& file);
    Asset LoadBinaryFile(std::filesystem::path const& Path);
    AssetRef LoadBinaryFileRef(std::filesystem::path const& Path);
}
