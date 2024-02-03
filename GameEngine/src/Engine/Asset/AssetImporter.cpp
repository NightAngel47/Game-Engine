#include "enginepch.h"
#include "Engine/Asset/AssetImporter.h"
#include "Engine/Asset/Assets.h"

#include "Engine/Asset/SceneImporter.h"
#include "Engine/Asset/TextureImporter.h"
#include "Engine/Asset/PrefabImporter.h"
#include "Engine/Asset/AudioImporter.h"

#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Renderer/TextureSerializer.h"
#include "Engine/Scene/PrefabSerializer.h"

namespace Engine
{
	using AssetSaveFunction = std::function<void(const AssetMetadata& metadata, const Ref<Asset>& asset)>;
	static std::unordered_map<AssetType, AssetSaveFunction> s_AssetSaveFunctions = 
	{
		{ AssetType::Scene,		SceneImporter::SaveScene},
		{ AssetType::Texture2D,	TextureImporter::SaveTexture2D},
		{ AssetType::Prefab,	PrefabImporter::SavePrefab},
		{ AssetType::AudioClip,	AudioImporter::SaveAudioClip}
	};

	using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
	static std::unordered_map<AssetType, AssetImportFunction> s_AssetImportFunctions = 
	{
		{ AssetType::Scene,		SceneImporter::ImportScene},
		{ AssetType::Texture2D,	TextureImporter::ImportTexture2D},
		{ AssetType::Prefab,	PrefabImporter::ImportPrefab},
		{ AssetType::AudioClip,	AudioImporter::ImportAudioClip}
	};

	Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
		{
			ENGINE_CORE_ERROR("No importer available for asset type: {}", Utils::AssetTypeToString(metadata.Type));
			return nullptr;
		}

		return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
	}

	void AssetImporter::SaveAsset(const AssetMetadata& metadata, const Ref<Asset>& asset)
	{
		if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
		{
			ENGINE_CORE_ERROR("No save available for asset type: {}", Utils::AssetTypeToString(metadata.Type));
			return;
		}

		s_AssetSaveFunctions.at(metadata.Type)(metadata, asset);
	}
}
