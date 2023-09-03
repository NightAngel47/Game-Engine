#include "enginepch.h"
#include "Engine/Asset/AssetImporter.h"
#include "Engine/Asset/Assets.h"

#include "Engine/Asset/SceneImporter.h"
#include "Engine/Asset/TextureImporter.h"
#include "Engine/Asset/PrefabImporter.h"

#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Renderer/TextureSerializer.h"
#include "Engine/Scene/PrefabSerializer.h"

namespace Engine
{
	static std::unordered_map<AssetType, Ref<AssetSerializer>> s_Serializers;

	using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
	static std::unordered_map<AssetType, AssetImportFunction> s_AssetImportFunctions = 
	{
		{ AssetType::Scene,		SceneImporter::ImportScene},
		{ AssetType::Texture2D,	TextureImporter::ImportTexture2D},
		{ AssetType::Prefab,	PrefabImporter::ImportPrefab}
	};

	void AssetImporter::Init()
	{
		s_Serializers[AssetType::Scene]		= CreateScope<SceneSerializer>();
		s_Serializers[AssetType::Texture2D]	= CreateScope<TextureSerializer>();
		s_Serializers[AssetType::Prefab]	= CreateScope<PrefabSerializer>();
	}

	void AssetImporter::Shutdown()
	{

	}

	Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
		{
			ENGINE_CORE_ERROR("No importer availabel for asset type: {}", Utils::AssetTypeToString(metadata.Type));
			return nullptr;
		}

		return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
	}

	void AssetImporter::SerializeAsset(const AssetMetadata& metadata, const Ref<Asset>& asset)
	{
		s_Serializers.at(metadata.Type)->Serialize(metadata, asset);
	}
}
