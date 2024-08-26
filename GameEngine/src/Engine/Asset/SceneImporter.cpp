#include "enginepch.h"
#include "Engine/Asset/SceneImporter.h"
#include "Engine/Scene/SceneSerializer.h"

namespace Engine
{
	Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		Ref<Asset> scene = CreateRef<Scene>();
		SceneSerializer serializer = SceneSerializer();
		if (serializer.TryLoadData(metadata, scene))
		{
			scene->Handle = handle;
			return As<Scene>(scene);
		}
		
		return nullptr;
	}

	Ref<Scene> SceneImporter::ImportSceneFromPak(AssetHandle handle, const PakAssetEntry& pakEntry)
	{
		Ref<Asset> scene = CreateRef<Scene>();
		SceneSerializer serializer = SceneSerializer();
		if (serializer.TryLoadData(pakEntry, scene))
		{
			scene->Handle = handle;
			return As<Scene>(scene);
		}

		return nullptr;
	}

	void SceneImporter::SaveScene(const AssetMetadata& metadata, const Ref<Asset>& asset)
	{
		SceneSerializer serializer = SceneSerializer();
		serializer.Serialize(metadata, asset);
	}

}
