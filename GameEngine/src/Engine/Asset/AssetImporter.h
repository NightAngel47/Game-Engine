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

	class AssetImporter
	{
	public:
		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);

		static void SaveAsset(const AssetMetadata& metadata, const Ref<Asset>& asset);
	};

}
