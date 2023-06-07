#pragma once
#include "Engine/Asset/Assets.h"
#include "Engine/Asset/AssetImporter.h"

namespace Engine
{
	using AssetMap = std::unordered_map<AssetHandle, Ref<Asset>>;

	class AssetManagerBase
	{
	public:
		//virtual Ref<Asset> GetAsset(AssetHandle handle) const = 0;
		virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;
		//virtual Ref<Asset> GetAsset(const std::filesystem::path& path) const = 0;

		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;

		virtual const AssetMetadata& GetAssetMetadata(AssetHandle handle) const = 0;

		//virtual AssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& path);
		//virtual void SaveAsset(const AssetMetadata& metadata, const Ref<Asset>& asset) const = 0;
		virtual void SaveAsset(const Ref<Asset>& asset) = 0;
	};
}
