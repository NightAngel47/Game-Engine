#pragma once
#include "Engine/Asset/AssetManagerBase.h"

namespace Engine
{
	using AssetRegistry = std::unordered_map<AssetHandle, AssetMetadata>;

	class EditorAssetManager : public AssetManagerBase
	{
	public:
		EditorAssetManager();

		Ref<Asset> GetAsset(AssetHandle handle) override;

		bool IsAssetHandleValid(AssetHandle handle) const override;
		bool IsAssetLoaded(AssetHandle handle) const override;

		const AssetMetadata& GetAssetMetadata(AssetHandle handle) const override;
		const AssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& path);

		void SaveAssetAs(const Ref<Asset>& asset, const std::filesystem::path& path);
		void SaveAsset(const Ref<Asset>& asset) override;

		void SaveAssetToRegistry(AssetHandle handle, const AssetMetadata& metadata);
	private:
		AssetMap m_LoadedAssets;
		AssetRegistry m_AssetRegistry;
	};
}
