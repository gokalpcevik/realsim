#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace RSim::AssetLib
{
	enum class CompressionMode
	{
		None = 0,
		LZ4
	};

	struct Asset
	{
		Asset() = default;
		Asset(Asset const&) = default;
		Asset& operator=(Asset const&) = default;
		~Asset() = default;

		Asset(Asset&& rhs) noexcept
		{
			// Probably faster than std::memcpy() ??
			Type[0] = rhs.Type[0];
			Type[1] = rhs.Type[1];
			Type[2] = rhs.Type[2];
			Type[3] = rhs.Type[3];
			Version = rhs.Version;
			Metadata = std::move(rhs.Metadata);
			BinaryBlob = std::move(rhs.BinaryBlob);
		}

		Asset& operator=(Asset&& rhs) noexcept
		{
			// Probably faster than std::memcpy() ??
			Type[0] = rhs.Type[0];
			Type[1] = rhs.Type[1];
			Type[2] = rhs.Type[2];
			Type[3] = rhs.Type[3];
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
		 * \brief Raw data of the object, like the pixels of a texture or vertices of a mesh file.
		 */
		std::vector<char> BinaryBlob{};

        static Asset const Null;
    };

	bool SaveBinaryFile(std::filesystem::path const& Path, Asset const& file);
	Asset LoadBinaryFile(std::filesystem::path const& Path);
}