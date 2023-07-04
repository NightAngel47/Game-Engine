#include "enginepch.h"
#include "Engine/Asset/EditorAssetManager.h"
#include "Engine/Asset/AssetRegistrySerializer.h"
#include "Engine/Project/Project.h"

namespace Engine
{
	static std::map<std::filesystem::path, AssetType> s_AssetExtensionMap =
	{
		{".scene", AssetType::Scene},

		{".png", AssetType::Texture2D},
		{".jpg", AssetType::Texture2D},
		{".jpeg", AssetType::Texture2D},

		{".prefab", AssetType::Prefab},

		{".cs", AssetType::ScriptFile}
	};

	static const AssetType GetAssetTypeFromFileExtension(const std::filesystem::path& extension)
	{
		if (s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
		{
			ENGINE_CORE_WARN("Could not find asset type based on extension: {}", extension);
			return AssetType::None;
		}
		
		return s_AssetExtensionMap.at(extension);
	}

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


	const AssetType EditorAssetManager::GetAssetType(AssetHandle handle) const
	{
		if (IsAssetHandleValid(handle))
			return GetAssetMetadata(handle).Type;

		return AssetType::None;
	}


	const std::filesystem::path& EditorAssetManager::GetAssetPath(AssetHandle handle) const
	{
		if (IsAssetHandleValid(handle))
			return GetAssetMetadata(handle).Path;

		return "";
	}

	const AssetHandle EditorAssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& path)
	{
		for (const auto& [handle, metadata] : m_AssetRegistry)
			if (metadata.Path == path)
				return handle;

		return AssetHandle::INVALID();
	}


	void EditorAssetManager::ImportAsset(const std::filesystem::path& path)
	{
		std::filesystem::path relativePath = std::filesystem::relative(path, Project::GetAssetDirectory());

		AssetHandle handle = GetAssetHandleFromFilePath(relativePath);

		if (handle.IsValid())
		{
			ENGINE_CORE_WARN("Asset is already imported: {}", path);
			return;
		}

		handle = AssetHandle(); // generate new handle for asset

		AssetMetadata metadata = AssetMetadata();
		metadata.Path = relativePath;
		metadata.Type = GetAssetTypeFromFileExtension(relativePath.extension());
		ENGINE_CORE_ASSERT(metadata.Type != AssetType::None, "Asset type was None when importing!");

		Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);
		asset->Handle = handle;
		if (asset)
			SaveAssetToRegistry(handle, metadata);
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

		if (IsAssetLoaded(handle))
			m_LoadedAssets[handle] = asset;

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
