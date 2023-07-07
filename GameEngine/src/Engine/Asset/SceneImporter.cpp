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

}
