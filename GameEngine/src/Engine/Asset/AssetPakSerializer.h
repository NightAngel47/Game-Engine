#pragma once
#include "Engine/Asset/AssetManagerBase.h"

//https://simoncoenen.com/blog/programming/PakFiles

namespace Engine
{
	struct PakHeader
	{
		char ID[4] = { "PAK" };
		char PakVersion = 0;
		uint32_t ContentVersion = 0;
		//char FolderPath[100];
		//char PakName[50];
		uint32_t NumEnteries = 0;
	};

	struct PakFileTableEntry
	{
		char FilePath[255];
		bool Compressed = false;
		uint32_t UncompressedSize = 0;
		uint32_t CompressedSize = 0;
		uint32_t OffSet = 0;
	};

	class AssetPakSerializer
	{
	public:
		AssetPakSerializer() = default;

		void Serialize(const AssetRegistry& assetRegistry);
		bool TryLoadData(AssetPak& assetPak);
	};
}
