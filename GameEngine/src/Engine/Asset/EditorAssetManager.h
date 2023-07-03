#pragma once
#include "Engine/Asset/AssetManagerBase.h"

#include <map>

namespace Engine
{
	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

	class EditorAssetManager : public AssetManagerBase
	{
	public:
		EditorAssetManager();

		Ref<Asset> GetAsset(AssetHandle handle) override;

		bool IsAssetHandleValid(AssetHandle handle) const override;
		bool IsAssetLoaded(AssetHandle handle) const override;

		const AssetMetadata& GetAssetMetadata(AssetHandle handle) const;
		const AssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& path);

		void ImportAsset(const std::filesystem::path& path);

		void SaveAssetAs(const Ref<Asset>& asset, const std::filesystem::path& path);
		void SaveAsset(const Ref<Asset>& asset);

		void SaveAssetToRegistry(AssetHandle handle, const AssetMetadata& metadata);

		const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; }
		const AssetMap& GetLoadedAssets() const { return m_LoadedAssets; }
	private:
		AssetMap m_LoadedAssets;
		AssetRegistry m_AssetRegistry;
	};
}
