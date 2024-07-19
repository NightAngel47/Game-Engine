#pragma once
#include "Engine/Asset/Assets.h"

namespace Engine
{
	class AssetSerializer
	{
	public:
		//TODO: Remove? Reevaluate asset pipeline
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const = 0;
		//TODO: Remove? Reevaluate asset pipeline
		virtual const std::vector<char> SerializeForStream(const AssetMetadata& metadata, const Ref<Asset>& asset) const = 0;
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const = 0;
		virtual bool TryLoadData(const PakAssetEntry& pakEntry, Ref<Asset>& asset) const = 0;
	};

	class AssetImporter
	{
	public:
		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Asset> ImportAsset(AssetHandle handle, const PakAssetEntry& pakEntry);

		static void SaveAsset(const AssetMetadata& metadata, const Ref<Asset>& asset);
	};

}
