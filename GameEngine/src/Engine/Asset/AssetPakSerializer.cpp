#include "enginepch.h"
#include "Engine/Asset/AssetPakSerializer.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Scene/PrefabSerializer.h"
#include "Engine/Scene/Prefab.h"
#include "Engine/Utils/FileSystem.h"

#include <yaml-cpp/yaml.h>

namespace Engine
{
	void AssetPakSerializer::Serialize(const AssetRegistry& assetRegistry)
	{
		// Create header
		PakHeader header = {};
		header.PakVersion = PAK_VERSION; // TODO make proper pak version number
		header.ContentVersion = 1; // TODO make proper content version number
		header.NumEnteries = 0;

		std::vector<PakAssetEntry> fileEntries;
		std::vector<char> dataBuffer = {};

		for (const auto& [handle, metadata] : assetRegistry)
		{
			if (handle == AssetHandle::INVALID())
			{
				ENGINE_CORE_WARN("Asset Handle: {} was invalid", handle);
				continue;
			}

			std::filesystem::path assetPath = Project::GetActiveAssetFileSystemPath(metadata.Path);
			bool isTextFile = metadata.Type == AssetType::Scene || metadata.Type == AssetType::Prefab;
			uint32_t fileSize = 0;
			std::vector<char> fileData;
			if (isTextFile)
			{
				switch (metadata.Type)
				{
					case AssetType::Scene:
					{
						SceneSerializer serializer = SceneSerializer();
						fileData = serializer.SerializeForStream(metadata, AssetManager::GetAsset<Scene>(handle));
						break;
					}
					case AssetType::Prefab:
					{
						PrefabSerializer serializer = PrefabSerializer();
						fileData = serializer.SerializeForStream(metadata, AssetManager::GetAsset<Prefab>(handle));
						break;
					}
					default:
					{
						ENGINE_CORE_ERROR("Asset is a text file type, but is not setup for binary serialization!");
						continue;
					}
				}

				fileSize = fileData.size();
				if (fileSize == 0)
				{
					// File is empty
					ENGINE_CORE_WARN("File is empty!");
					continue;
				}
			}
			else
			{
				std::ifstream fileStream(assetPath, std::ios::binary);
				if (fileStream.fail())
				{
					// Failed to open the file
					ENGINE_CORE_WARN("Failed to open the file!");
					continue;
				}

				fileSize = std::filesystem::file_size(assetPath);
				if (fileSize == 0)
				{
					// File is empty
					ENGINE_CORE_WARN("File is empty!");
					continue;
				}

				// Read file into memory
				fileData.resize(fileSize);
				fileStream.read(fileData.data(), fileSize);
				fileStream.close();
			}

			// Create the file entry
			PakAssetEntry pakFileEntry = {};
			pakFileEntry.Handle = handle;
			pakFileEntry.Type = metadata.Type;
			pakFileEntry.UncompressedSize = fileSize;
			pakFileEntry.OffSet = dataBuffer.size();
			pakFileEntry.Compressed = metadata.Compress;

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
			header.NumEnteries++;
		}

		std::filesystem::path assetPakPath = Project::GetActiveAssetPakPath();
		ENGINE_CORE_WARN("Creating Pak File at: {}", assetPakPath);
		std::ofstream pakFile(assetPakPath, std::ios::binary);
		if (pakFile.fail())
		{
			ENGINE_CORE_WARN("Failed to create the file!");
			return;
		}

		ENGINE_CORE_WARN("Writing Pak File to: {}", assetPakPath);
		pakFile.write(reinterpret_cast<char const*>(&header), sizeof(header));
		for each (PakAssetEntry entry in fileEntries)
			pakFile.write(reinterpret_cast<char const*>(&entry), sizeof(entry));
		pakFile.write(dataBuffer.data(), dataBuffer.size());
		pakFile.close();
		ENGINE_CORE_WARN("Finished Writing Pak File to: {}", assetPakPath);
	}

	bool AssetPakSerializer::TryLoadData(AssetPak& assetPak)
	{
		std::filesystem::path assetPakPath = Project::GetActiveAssetPakPath();
		if (!std::filesystem::exists(assetPakPath))
		{
			ENGINE_CORE_ERROR("File doesn't exist!");
			return false;
		}

		std::ifstream fileStream(assetPakPath, std::ios::binary);
		if (fileStream.fail())
		{
			ENGINE_CORE_ERROR("Failed to open the file!");
			return false;
		}

		PakHeader header = {};
		if (!fileStream.read(reinterpret_cast<char*>(&header), sizeof(header)))
		{
			ENGINE_CORE_ERROR("Failed to get Header!");
			return false;
		}
		if (std::strcmp("GPK", header.ID) != 0)
		{
			ENGINE_CORE_ERROR("File is not an AssetPak!");
			return false;
		}
		if (header.PakVersion != PAK_VERSION)
		{
			ENGINE_CORE_ERROR("AssetPak Version incompatible!");
			return false;
		}

		std::vector<PakAssetEntry> fileEntries;
		fileEntries.resize(header.NumEnteries);
		PakAssetEntry entry = {};
		for (uint32_t i = 0; i < header.NumEnteries; i++)
		{
			fileStream.seekg(sizeof(header) + i * sizeof(PakAssetEntry));
			fileStream.read(reinterpret_cast<char*>(&entry), sizeof(PakAssetEntry));
			fileEntries[i] = entry;
		}
		/*
		if (!fileStream.read(reinterpret_cast<char*>(fileEntries.data()), header.NumEnteries * sizeof(PakAssetEntry)))
		{
			ENGINE_CORE_ERROR("Failed to get file entries!");
			return false;
		}
		*/

		for each (const PakAssetEntry entry in fileEntries)
		{
			ENGINE_CORE_TRACE("Found AssetHandle {}", entry.Handle);
			assetPak[entry.Handle] = entry;
		}

		return true;
	}
}
