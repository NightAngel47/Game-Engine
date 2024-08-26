#pragma once
#include "Engine/Scene/Prefab.h"

namespace Engine
{
	class PrefabImporter
	{
	public:
		static Ref<Prefab> ImportPrefab(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Prefab> ImportPrefabFromPak(AssetHandle handle, const PakAssetEntry& pakEntry);
		static void SavePrefab(const AssetMetadata& metadata, const Ref<Asset>& asset);
	};
}
