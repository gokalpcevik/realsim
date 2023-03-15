#pragma once

// Standard library includes
#include <filesystem>
#include <fstream>

// Third Party
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <nlohmann/json.hpp>
#include <lz4.h>

#include "assetbaker/Logger.h"
#include "assetbaker/Defaults.h"
#include "assetbaker/Timer.h"

#include "assetlib/MeshLoader.h"

namespace RSim::AssetBaker
{
	struct MeshImportOptions
	{
		/**
		 * \brief This determines whether the meshes in the 3D scene file(.fbx, .obj) should get their data merged into a single asset file.
		 * If this is true, then an entire scene consisting of multiple meshes will get treated as a single mesh in the rendering engine.
		 */
		bool MergeIntoSingleFile = false;
	};

	class MeshBaker
	{
	public:
		/**
		 * \param FilePath Path to the mesh/scene file.
		 * \param Options Mesh import options specified by the user.
		 * \param OutputDir Output directory.
		 */
		explicit MeshBaker(std::filesystem::path const & FilePath,MeshImportOptions Options,std::filesystem::path const& OutputDir = DefaultOutputDir);
	private:
		// aiFace consists of two uint32_t's so it might be better to pass it by value.
		// but clang-tidy says to pass by const ref. so I dunno
		// TODO: Investigate.
		static inline void InitIndicesFromFace(std::vector<uint32_t>& indices, aiFace const& pFace, size_t BaseFaceIndex);

        static inline void InitVertexFromAssimp(AssetLib::Vertex_F32PNCV& Vertex,
                                                aiVector3D const& Position,
                                                aiVector3D const& Normal,
                                                aiColor3D const& Color,
                                                aiVector3D const& UV);
		static inline AssetLib::MeshInfo CreateMeshInfo(std::vector<AssetLib::Vertex_F32PNCV> const& Vertices, std::vector<uint32_t> const& Indices, std::string const& OriginalFile);
	};
}
