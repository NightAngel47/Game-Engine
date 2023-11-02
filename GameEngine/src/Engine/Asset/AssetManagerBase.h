#pragma once
#include "Engine/Asset/Assets.h"
#include "Engine/Asset/AssetImporter.h"

namespace Engine
{
	using AssetMap = std::unordered_map<AssetHandle, Ref<Asset>>;

	class AssetManagerBase
	{
	public:
		virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;

		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;

		virtual const std::unordered_map<AssetHandle, AssetMetadata>& GetAssets() const = 0;
	};
}
