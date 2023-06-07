#pragma once
#include "Engine/Asset/Assets.h"

namespace Engine
{
	class AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const = 0;
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const = 0;
	};

	using AssetSerializerMap = std::unordered_map<AssetType, Ref<AssetSerializer>>;

	class AssetImporter
	{
	public:
		static void Init();
		static void Shutdown();

		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);

		static void SerializeAsset(const AssetMetadata& metadata, const Ref<Asset>& asset);
	};

}
