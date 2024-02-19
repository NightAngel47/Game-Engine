#pragma once
#include "Engine/Asset/Assets.h"
#include "Engine/Asset/AssetImporter.h"

namespace Engine
{
	using AssetMap = std::unordered_map<AssetHandle, Ref<Asset>>;	
	using AssetRegistry = std::unordered_map<AssetHandle, AssetMetadata>;
	using AssetPak = std::unordered_map<AssetHandle, AssetMetadata>; // TODO make proper asset pack (using asset registry atm)

	class AssetManagerBase
	{
	public:
		virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;

		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;

		virtual const std::unordered_map<AssetHandle, AssetMetadata>& GetAssets() const = 0;
	};
}
