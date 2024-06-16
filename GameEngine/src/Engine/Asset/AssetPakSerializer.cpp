#include "enginepch.h"
#include "Engine/Asset/AssetPakSerializer.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Utils/FileSystem.h"

namespace Engine
{

	void AssetPakSerializer::Serialize(const AssetRegistry& assetRegistry)
	{
		// Create header
		PakHeader header = {};
		header.NumEnteries = 0;
		header.PakVersion = 1; // TODO make proper pak version number
		header.ContentVersion = 1; // TODO make proper content version number

		std::vector<PakFileTableEntry> fileEntries;
		std::vector<char> dataBuffer = {};

		for (const auto& [handle, metadata] : assetRegistry)
		{
			if (handle == AssetHandle::INVALID())
			{
				ENGINE_CORE_WARN("Asset Handle: {} was invalid", handle);
				continue;
			}

			//Ref<Asset> asset = AssetManager::GetAsset<Asset>(handle);
			std::filesystem::path assetPath = Project::GetActiveAssetFileSystemPath(metadata.Path);
			std::ifstream fileStream(assetPath, std::ios::binary | std::ios::ate);
			if (fileStream.fail())
			{
				// Failed to open the file
				ENGINE_CORE_ERROR("Failed to open the file!");
				continue;
			}

			std::streampos end = fileStream.tellg();
			fileStream.seekg(0, std::ios::beg);
			uint32_t size = end - fileStream.tellg();
			if (size == 0)
			{
				// File is empty
				ENGINE_CORE_ERROR("File is empty!");
				continue;
			}

			//std::vector<char> dataBuffer = {};
			//dataBuffer.resize(size);
			//fileStream.read(dataBuffer.data(), size);

			// Create the file entry
			PakFileTableEntry pakFileEntry = {};
			pakFileEntry.UncompressedSize = size;//fileStream.tellg();
			pakFileEntry.OffSet = dataBuffer.size();
			pakFileEntry.Compressed = metadata.Compress;
			fileStream.seekg(0);

			//dataBuffer.resize(dataBuffer.size() + pakFileEntry.UncompressedSize);

			// Read file into memory
			std::vector<char> fileData;
			fileData.resize(pakFileEntry.UncompressedSize);
			fileStream.read(fileData.data(), pakFileEntry.UncompressedSize);

			// Compress the data
			if (pakFileEntry.Compressed)
			{
				// TODO add compression format

				// just putting the uncompressed way here for now TODO REMOVE
				pakFileEntry.CompressedSize = pakFileEntry.UncompressedSize;
				dataBuffer.insert(dataBuffer.end(), fileData.begin(), fileData.end());
			}
			else
			{
				pakFileEntry.CompressedSize = pakFileEntry.UncompressedSize;
				dataBuffer.insert(dataBuffer.end(), fileData.begin(), fileData.end());
			}

			// Add entry and close file stream
			fileEntries.push_back(pakFileEntry);
			fileStream.close();
			header.NumEnteries++;
		}

		std::filesystem::path assetPakDir = Project::GetActiveProjectDirectory().generic_string() + "/Paks";
		if (!std::filesystem::exists(assetPakDir))
			std::filesystem::create_directories(assetPakDir);

		ENGINE_CORE_WARN("Writing Pak File to: {}", assetPakDir.generic_string() + "/AssetPak.pak");
		std::ofstream pakFile(assetPakDir.generic_string() + "/AssetPak.pak", std::ios::binary);
		if (pakFile.fail())
		{
			ENGINE_CORE_ERROR("Failed to open the file!");
			return;
		}
		pakFile.write(reinterpret_cast<char const*>(&header), sizeof(header));
		pakFile.write(dataBuffer.data(), dataBuffer.size());
		pakFile.close();
	}

	bool AssetPakSerializer::TryLoadData(AssetPak& assetPak)
	{
		return false;
	}
}
