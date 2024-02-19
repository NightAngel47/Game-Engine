#include "enginepch.h"
#include "Engine/Asset/RuntimeAssetManager.h"
#include "Engine/Asset/AssetRegistrySerializer.h"

namespace Engine
{
	RuntimeAssetManager::RuntimeAssetManager()
	{
		AssetRegistrySerializer assetRegistrySerializer = AssetRegistrySerializer();
		if (!assetRegistrySerializer.TryLoadData(m_AssetPak))
		{
			ENGINE_CORE_WARN("Asset Registry Failed to Load");

			ENGINE_CORE_INFO("Creating Empty Asset Registry");
			m_AssetPak = AssetRegistry();
		}

		m_LoadedAssets = AssetMap();
	}

	Ref<Asset> RuntimeAssetManager::GetAsset(AssetHandle handle)
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


	bool RuntimeAssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return handle.IsValid() && m_AssetPak.find(handle) != m_AssetPak.end();
	}


	bool RuntimeAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return handle.IsValid() && m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}


	const AssetMetadata& RuntimeAssetManager::GetAssetMetadata(AssetHandle handle) const
	{
		static AssetMetadata s_NullMetadata;
		auto it = m_AssetPak.find(handle);
		if (it == m_AssetPak.end())
			return s_NullMetadata;

		return it->second;
	}

}
