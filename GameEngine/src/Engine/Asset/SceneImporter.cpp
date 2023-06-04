#include "enginepch.h"
#include "Engine/Asset/SceneImporter.h"
#include "Engine/Scene/SceneSerializer.h"

namespace Engine
{
	Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata, bool isResource)
	{
		Ref<Scene> scene = CreateRef<Scene>();
		SceneSerializer serializer = SceneSerializer();
		if (serializer.TryLoadData(metadata, scene))
			return scene;
		
		return nullptr;
	}

}
