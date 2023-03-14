#include "assetlib/MeshLoader.h"

namespace RSim::AssetLib
{
	CompressionMode GetCompressionModeFromString(std::string_view str)
	{
		return CompressionMode::LZ4;
	}

	MeshInfo ReadMeshInfo(AssetLib::Asset const& AssetFile)
	{
		MeshInfo info;
		nlohmann::json metadata;
		try
		{
			metadata = nlohmann::json::parse(AssetFile.Metadata);
		}
		catch(nlohmann::json::exception const& e)
		{
			std::cerr << e.what() << std::endl;
			throw std::exception();
		}

		info.VertexBufferSizeInBytes = metadata["vertex_buffer_size"];
		info.IndexBufferSizeInBytes = metadata["index_buffer_size"];
		info.IndexSize = (uint8_t)metadata["index_size"];
		info.IndexCount = metadata["index_count"];
		info.OriginalFile = metadata["original_file"];

		std::string compressionString = metadata["compression"];
		info.CompressionMode = GetCompressionModeFromString(compressionString.c_str());

		return info;
	}

	Asset PackMesh(MeshInfo const &Info, char const* pVertexData, char const* pIndexData)
	{
		Asset file;
		file.Type[0] = 'M';
		file.Type[1] = 'E';
		file.Type[2] = 'S';
		file.Type[3] = 'H';
		file.Version = 1;

		nlohmann::json metadata;
		metadata["vertex_format"] = "F32_PNCV";
		metadata["vertex_buffer_size"] = Info.VertexBufferSizeInBytes;
		metadata["index_buffer_size"] = Info.IndexBufferSizeInBytes;
		metadata["index_size"] = Info.IndexSize;
		metadata["index_count"] = Info.IndexCount;
		metadata["original_file"] = Info.OriginalFile;

		size_t fullsize = Info.VertexBufferSizeInBytes + Info.IndexBufferSizeInBytes;

		std::vector<char> MergedBuffer;
		MergedBuffer.resize(fullsize);

		//copy vertex buffer
		memcpy(MergedBuffer.data(), pVertexData, Info.VertexBufferSizeInBytes);

		//copy index buffer
		memcpy(MergedBuffer.data() + Info.VertexBufferSizeInBytes, pIndexData, Info.IndexBufferSizeInBytes);

		//compress buffer and copy it into the file struct
		size_t compressStaging = LZ4_compressBound(static_cast<int>(fullsize));

		file.BinaryBlob.resize(compressStaging);
		
		int compressedSize = LZ4_compress_default(MergedBuffer.data(), 
			file.BinaryBlob.data(), 
			static_cast<int>(MergedBuffer.size()), 
			static_cast<int>(compressStaging));
		file.BinaryBlob.resize(compressedSize);

		metadata["compression"] = "LZ4";

		file.Metadata = metadata.dump();

		return file;
	}

	void UnpackMesh(MeshInfo const& Info, const char* SourceBuffer, size_t SourceSize, char* VertexBuffer, char* IndexBuffer)
	{
		std::vector<char> decompressedBuffer;
		decompressedBuffer.resize(Info.VertexBufferSizeInBytes + Info.IndexBufferSizeInBytes);

		LZ4_decompress_safe(SourceBuffer, decompressedBuffer.data(), static_cast<int>(SourceSize), static_cast<int>(decompressedBuffer.size()));

		//copy vertex buffer
		memcpy(VertexBuffer, decompressedBuffer.data(), Info.VertexBufferSizeInBytes);

		//copy index buffer
		memcpy(IndexBuffer, decompressedBuffer.data() + Info.VertexBufferSizeInBytes, Info.IndexBufferSizeInBytes);
	}

	void UnpackMesh(MeshInfo const& Info, Asset const& Asset, UnpackedMesh& OutUnpacked)
	{
		OutUnpacked.VertexBufferBlob.resize(Info.VertexBufferSizeInBytes);
		OutUnpacked.IndexBufferBlob.resize(Info.IndexBufferSizeInBytes);
		AssetLib::UnpackMesh(Info, Asset.BinaryBlob.data(), Asset.BinaryBlob.size(), OutUnpacked.VertexBufferBlob.data(), OutUnpacked.IndexBufferBlob.data());
	}

	std::pair<Vertex_F32PNCV*, size_t> ExtractVerticesFromUnpackedMesh(UnpackedMesh const& UnpackedMesh)
	{
		return ExtractVerticesFromUnpackedMesh(UnpackedMesh.VertexBufferBlob);
	}

	std::pair<Vertex_F32PNCV*, size_t> ExtractVerticesFromUnpackedMesh(std::vector<char> const& UnpackedVertexBuffer)
	{
		assert(UnpackedVertexBuffer.size() % MeshInfo::VertexStride == 0);
		Vertex_F32PNCV* pVertices = const_cast<Vertex_F32PNCV*>(reinterpret_cast<Vertex_F32PNCV const*>(UnpackedVertexBuffer.data()));
		return { pVertices, UnpackedVertexBuffer.size() / MeshInfo::VertexStride }; 
	}
}
