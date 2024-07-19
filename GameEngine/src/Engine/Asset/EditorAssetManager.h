#pragma once
#include "Engine/Asset/AssetManagerBase.h"

namespace Engine
{
	class EditorAssetManager : public AssetManagerBase
	{
	public:
		EditorAssetManager();

		Ref<Asset> GetAsset(AssetHandle handle) override;

		bool IsAssetHandleValid(AssetHandle handle) const override;
		bool IsAssetLoaded(AssetHandle handle) const override;

		const AssetMetadata& GetAssetMetadata(AssetHandle handle) const;
		const AssetType GetAssetType(AssetHandle handle) const;
		const std::filesystem::path& GetAssetPath(AssetHandle handle) const;
		const AssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& path);

		void ImportAsset(const std::filesystem::path& path);

		void SaveAssetAs(const Ref<Asset>& asset, const std::filesystem::path& path);
		void SaveAsset(const Ref<Asset>& asset);

		void SaveAssetToRegistry(AssetHandle handle, const AssetMetadata& metadata);

		const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; }
		const AssetMap& GetLoadedAssets() const { return m_LoadedAssets; }

		const AssetType GetAssetTypeFromFileExtension(const std::filesystem::path& extension);

		const AssetMap GetAssetsOfType(AssetType type) const override;

	private:
		AssetMap m_LoadedAssets;
		AssetRegistry m_AssetRegistry;
	};
}
