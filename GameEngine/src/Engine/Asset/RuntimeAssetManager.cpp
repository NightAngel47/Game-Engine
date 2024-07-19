#include "enginepch.h"
#include "Engine/Asset/RuntimeAssetManager.h"
#include "Engine/Asset/AssetPakSerializer.h"
#include "Engine/Project/Project.h"

namespace Engine
{
	RuntimeAssetManager::RuntimeAssetManager()
	{
		AssetPakSerializer assetPakSerializer = AssetPakSerializer();
		bool didLoad = assetPakSerializer.TryLoadData(m_AssetPak);
		ENGINE_CORE_ASSERT(didLoad, "Failed to load Asset Pak!");

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
			asset = AssetImporter::ImportAsset(handle, m_AssetPak[handle]); //TODO runtime version
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

	const AssetMap RuntimeAssetManager::GetAssetsOfType(AssetType type) const
	{
		AssetMap assets = {};
		for (const auto& [handle, pakEntry] : m_AssetPak)
		{
			if (pakEntry.Type == type)
				assets[handle] = Project().GetActive()->GetRuntimeAssetManager()->GetAsset(handle);
		}

		return assets;
	}
}
