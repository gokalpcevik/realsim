#include "assetlib/AssetLoader.h"


namespace RSim::AssetLib
{
    Asset const Asset::Null = {};

	bool SaveBinaryFile(std::filesystem::path const& Path, Asset const& file)
	{
		std::ofstream outfile;

		outfile.open(Path, std::ios::binary | std::ios::out);
		if (!outfile.is_open())
		{
			std::cerr << "Error when trying to write file: " << Path << std::endl;
		}
		outfile.write(file.Type, 4);
		uint32_t version = file.Version;
		//version
		outfile.write((const char*)&version, sizeof(uint32_t));

		// Metadata length in bytes
		uint32_t lenght = static_cast<uint32_t>(file.Metadata.size());
		outfile.write((const char*)&lenght, sizeof(uint32_t));

		// Blob length in bytes
		uint32_t bloblenght = static_cast<uint32_t>(file.BinaryBlob.size());
		outfile.write((const char*)&bloblenght, sizeof(uint32_t));

		// JSON metadata stream
		outfile.write(file.Metadata.data(), lenght);
		outfile.write(file.BinaryBlob.data(), (uint32_t)file.BinaryBlob.size());

		outfile.close();

		return true;
	}

	Asset LoadBinaryFile(std::filesystem::path const& Path)
	{
		Asset asset;
		std::ifstream infile;
		infile.open(Path, std::ios::binary);

		if (!infile.is_open()) return Asset::Null;

		infile.seekg(0);

		infile.read(asset.Type, 4);

		infile.read((char*)&asset.Version, sizeof(uint32_t));

		uint32_t jsonLen = 0;
		infile.read((char*)&jsonLen, sizeof(uint32_t));

		uint32_t blobLen = 0;
		infile.read((char*)&blobLen, sizeof(uint32_t));

		asset.Metadata.resize(jsonLen);

		infile.read(asset.Metadata.data(), jsonLen);
			
		asset.BinaryBlob.resize(blobLen);
		infile.read(asset.BinaryBlob.data(), blobLen);

		asset.TotalCompressedSizeInBytes = GetFileSize(Path);
		asset.HashValue = hash_value(Path);

		return asset;
	}
}
