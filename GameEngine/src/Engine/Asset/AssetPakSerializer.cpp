#include "enginepch.h"
#include "Engine/Asset/AssetPakSerializer.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Utils/FileSystem.h"

namespace Engine
{
	static std::vector<PakFileTableEntry> s_FileEntries;

	void AssetPakSerializer::Serialize(const AssetRegistry& assetRegistry)
	{
		// Create header
		PakHeader header = {};
		header.NumEnteries = 0;
		header.PakVersion = 1; // TODO make proper pak version number
		header.ContentVersion = 1; // TODO make proper content version number

		s_FileEntries.clear();

		for (const auto& [handle, metadata] : assetRegistry)
		{
			if (handle == AssetHandle::INVALID())
			{
				ENGINE_CORE_WARN("Asset Handle: {} was invalid", handle);
				continue;
			}

			std::ifstream fileStream(Project::GetActiveAssetFileSystemPath(metadata.Path), std::ios::binary | std::ios::ate);
			if (!fileStream)
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

			std::vector<char> dataBuffer = {};
			dataBuffer.resize(size);
			fileStream.read(dataBuffer.data(), size);

			//Ref<Asset> asset = AssetManager::GetAsset<Asset>(handle);

			// Create the file entry
			PakFileTableEntry pakFileEntry = {};
			pakFileEntry.UncompressedSize = fileStream.tellg();
			pakFileEntry.OffSet = dataBuffer.size();
			pakFileEntry.Compressed = metadata.Compress;
			fileStream.seekg(0);

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
			s_FileEntries.push_back(pakFileEntry);
			fileStream.close();
		}
	}

	bool AssetPakSerializer::TryLoadData(AssetPak& assetPak)
	{
		return false;
	}
}
