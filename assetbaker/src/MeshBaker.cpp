#include "assetbaker/MeshBaker.h"
#include "assetlib/FileHelper.h"

namespace RSim::AssetBaker
{
	namespace fs = std::filesystem;

	MeshBaker::MeshBaker(std::filesystem::path const& FilePath, 
		MeshImportOptions Options,
		fs::path const& OutputDir)
	{
		Assimp::Importer importer;
		ManualTimer timer;
		// ReadFile() takes so long to execute its ridiculous sometimes
		aiScene const* pScene = importer.ReadFile(FilePath.string(), aiProcess_ConvertToLeftHanded | aiProcess_Triangulate);
		if(pScene == nullptr)
		{
			baker_error("Can not read or recognize mesh file '{0}'.",FilePath.string());
			return;
		}

		auto Stem = FilePath.stem();
		

		// This part can probably be multi-threaded.
		for (size_t i = 0; i < pScene->mNumMeshes; ++i)
		{
			timer.Start(fmt::format("'{0}' Mesh File Conversion",Stem.string()));

			std::string FileNameWithExtension = Stem.string() + std::to_string(i) + ".rsim";
			fs::path FinalOutputPath = OutputDir / FileNameWithExtension;

			aiMesh* pMesh = pScene->mMeshes[i];
			std::vector<uint32_t> Indices;
			std::vector<AssetLib::Vertex_F32PNCV> Vertices;

			Indices.reserve(pMesh->mNumFaces * 3UL);
			Vertices.reserve(pMesh->mNumVertices);

			Indices.resize(pMesh->mNumFaces * 3UL);
			Vertices.resize(pMesh->mNumVertices);

			for (size_t FaceIndex = 0; FaceIndex < pMesh->mNumFaces; ++FaceIndex)
			{
				aiFace const& face = pMesh->mFaces[FaceIndex];
				SetIndicesFromFace(Indices, face, FaceIndex);
			}

			for (size_t VertexIndex = 0; VertexIndex < pMesh->mNumVertices; ++VertexIndex)
			{
				aiVector3D Position = pMesh->mVertices[VertexIndex];
				aiVector3D Normal = pMesh->mNormals[VertexIndex];

				aiVector3D const* UVCoordinates = pMesh->mTextureCoords[0];
				aiVector3D UV{0.0f, 0.0f, 0.0f};
				if (UVCoordinates)
					UV = *UVCoordinates;
				else
					baker_warn("The mesh '{0}' does not have any UV mapping. Defaulting to zero.", pMesh->mName.C_Str());

				AssetLib::Vertex_F32PNCV Vertex{};
				SetVertexFromAssimp(Vertex, Position, Normal, UV);
				Vertices[VertexIndex] = Vertex;
			}
			AssetLib::MeshInfo Info = GetMeshInfo(Vertices, Indices, FilePath.string());
			AssetLib::Asset asset = AssetLib::PackMesh(Info, reinterpret_cast<char*>(Vertices.data()), reinterpret_cast<char*>(Indices.data()));
			if(AssetLib::SaveBinaryFile(FinalOutputPath, asset))
			{
				// Although the outputted .rsim file might be larger than the original mesh/scene file, it will still be incredibly faster
				// to import to the engine because we skip the parsing part.
				int64_t OriginalSize = Info.VertexBufferSizeInBytes + Info.IndexBufferSizeInBytes;
				int64_t CompressedSize = asset.BinaryBlob.size();
				baker_trace("Original Size(Body): {} KB", OriginalSize / 1024);
				baker_trace("Compressed Size(Blob): {} KB", CompressedSize / 1024);
				baker_info("The mesh data was compressed by {0:.2f}%.", 100.0f - (float)CompressedSize / (float)OriginalSize * 100.0f);
				baker_info("{0} -> {1}",FilePath.string(), FinalOutputPath.string());
				timer.Stop();
			}
			else
			{
				baker_error("Error while saving compressed mesh file '{0}'.", FinalOutputPath.string());
				timer.Stop();
			}
		}
	}

	void MeshBaker::SetIndicesFromFace(std::vector<uint32_t>& indices, aiFace const& pFace, size_t BaseFaceIndex)
	{
		indices[BaseFaceIndex * 3 + 0] = pFace.mIndices[0];
		indices[BaseFaceIndex * 3 + 1] = pFace.mIndices[1];
		indices[BaseFaceIndex * 3 + 2] = pFace.mIndices[2];
	}

	void MeshBaker::SetVertexFromAssimp(AssetLib::Vertex_F32PNCV& Vertex, aiVector3D const& Position, aiVector3D const& Normal,
	                                    aiVector3D const& UV)
	{
		Vertex.UV[0] = UV.x;
		Vertex.UV[1] = UV.y;

		// Copying these is a good idea?
		// TODO: Investigate.
		Vertex.Position[0] = Position[0];
		Vertex.Position[1] = Position[1];
		Vertex.Position[2] = Position[2];

		Vertex.Normal[0] = Normal[0];
		Vertex.Normal[1] = Normal[1];
		Vertex.Normal[2] = Normal[2];
	}

	AssetLib::MeshInfo MeshBaker::GetMeshInfo(std::vector<AssetLib::Vertex_F32PNCV> const& Vertices,
	                                          std::vector<uint32_t> const& Indices, std::string const& OriginalFile)
	{
		AssetLib::MeshInfo Info;
		// TODO: Make this an option that can be taken in from the command line
		Info.CompressionMode = AssetLib::CompressionMode::LZ4;
		Info.IndexBufferSizeInBytes = Indices.size() * sizeof(uint32_t);
		Info.IndexSize = sizeof(uint32_t);
		Info.VertexBufferSizeInBytes = Vertices.size() * AssetLib::MeshInfo::VertexStride;
		Info.OriginalFile = OriginalFile;
		return Info;
	}
}
