#include "enginepch.h"
#include "Engine/Asset/EditorAssetManager.h"
#include "Engine/Asset/AssetRegistrySerializer.h"

namespace Engine
{
	EditorAssetManager::EditorAssetManager()
	{
		AssetRegistrySerializer assetRegistrySerializer = AssetRegistrySerializer();
		if (!assetRegistrySerializer.TryLoadData(m_AssetRegistry))
		{
			ENGINE_CORE_WARN("Asset Registry Failed to Load");

			ENGINE_CORE_INFO("Creating Empty Asset Registry");
			m_AssetRegistry = AssetRegistry();
		}

		m_LoadedAssets = AssetMap();
	}

	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
	{
		if (!IsAssetHandleValid(handle))
			return nullptr;

		Ref<Asset> asset;
		if (IsAssetLoaded(handle))
		{
			asset = m_LoadedAssets.at(handle);
		}
		else
		{
			const AssetMetadata& metadata = GetAssetMetadata(handle);
			asset = AssetImporter::ImportAsset(handle, metadata);
			m_LoadedAssets[handle] = asset;
		}

		return asset;
	}

	bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return handle.IsValid() && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
	}

	bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return handle.IsValid() && m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	const AssetMetadata& EditorAssetManager::GetAssetMetadata(AssetHandle handle) const
	{
		static AssetMetadata s_NullMetadata;
		auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end())
			return s_NullMetadata;

		return it->second;
	}

	const AssetHandle EditorAssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& path)
	{
		for (const auto& [handle, metadata] : m_AssetRegistry)
			if (metadata.Path == path)
				return handle;

		return AssetHandle::INVALID();
	}

	void EditorAssetManager::SaveAssetAs(const Ref<Asset>& asset, const std::filesystem::path& path)
	{
		AssetHandle handle = asset->Handle.IsValid() ? asset->Handle : AssetHandle();
		if (IsAssetHandleValid(handle))
		{
			SaveAsset(asset);
			return;
		}

		AssetMetadata metadata = AssetMetadata();
		metadata.Path = path;
		metadata.Type = asset->GetAssetType();

		m_AssetRegistry[handle] = metadata;

		AssetImporter::SerializeAsset(metadata, asset);
		SaveAssetToRegistry(handle, metadata);
	}

	void EditorAssetManager::SaveAsset(const Ref<Asset>& asset)
	{
		AssetHandle handle = asset->Handle;
		if (!IsAssetHandleValid(handle))
			return;

		const AssetMetadata& metadata = GetAssetMetadata(handle);

		AssetImporter::SerializeAsset(metadata, asset);
		SaveAssetToRegistry(handle, metadata);
	}


	void EditorAssetManager::SaveAssetToRegistry(AssetHandle handle, const AssetMetadata& metadata)
	{
		m_AssetRegistry[handle] = metadata;

		AssetRegistrySerializer assetRegistrySerializer = AssetRegistrySerializer();
		assetRegistrySerializer.Serialize(m_AssetRegistry);
	}

}
