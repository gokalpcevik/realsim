#pragma once
#include <cstdint>
#include <string>
#include <nlohmann/json.hpp>
#include <lz4.h>
#include "assetlib/AssetLoader.h"

namespace RSim::AssetLib
{
	struct Vertex_F32PNCV
	{
		float Position[3];
		float Normal[3];
		float Color[3];
		float UV[2];
	};

	struct MeshInfo
	{
		/**
		 * \brief Vertex buffer size, decompressed.
		 */
		size_t VertexBufferSizeInBytes{};
		/**
		 * \brief This is the size of a single vertex in bytes and it is equal to sizeof(Vertex_F32PNCV).
		 * This is currently hardcoded till we need some other formats.
		 */
		static size_t constexpr VertexStride = sizeof(Vertex_F32PNCV);
		/**
		 * \brief Index buffer size, decompressed.
		 */
		size_t IndexBufferSizeInBytes{};
		/**
		 * \brief Size of an index in bytes. It is usually equal sizeof(uint32_t) = 4 bytes. 
		 */
		char IndexSize{};
		/**
		 * \brief Path to the original file.
		 */
		std::string OriginalFile;
		/**
		 * \brief Compression mode.
		 */
		AssetLib::CompressionMode CompressionMode;
	};

	/**
	 * \brief Read the mesh info from the binary file. This function reads the metadata of the .rsim file and parses the JSON keys/values.
	 * \param AssetFile The binary file that was loaded into memory from disk.
	 * \return The parsed metadata values.
	 */
	MeshInfo ReadMeshInfo(AssetLib::Asset const& AssetFile);
	/**
	 * \brief Takes the vertex and index data, compresses them and encodes other information that are present in the binary file, like metadata and the asset type.
	 * \param Info Mesh info that will be used to construct the file metadata.
	 * \param pVertexData Vertex data that will be compressed.
	 * \param pIndexData Index data that will be compressed.
	 */
	Asset PackMesh(MeshInfo const& Info, char const* pVertexData, char const* pIndexData);
	void UnpackMesh(MeshInfo const& Info, const char* SourceBuffer, size_t SourceSize, char* VertexBuffer, char* IndexBuffer);

	struct UnpackedMesh
	{
		std::vector<char> VertexBufferBlob;
		std::vector<char> IndexBufferBlob;
	};

	void UnpackMesh(MeshInfo const& Info, Asset const& Asset, UnpackedMesh& OutUnpacked);

	std::pair<Vertex_F32PNCV*, size_t> ExtractVerticesFromUnpackedMesh(UnpackedMesh const& UnpackedMesh);
	std::pair<Vertex_F32PNCV*, size_t> ExtractVerticesFromUnpackedMesh(std::vector<char> const& UnpackedVertexBuffer);
}
