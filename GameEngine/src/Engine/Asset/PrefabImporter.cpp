#include "enginepch.h"
#include "Engine/Asset/PrefabImporter.h"
#include "Engine/Scene/PrefabSerializer.h"

namespace Engine
{
	Ref<Prefab> PrefabImporter::ImportPrefab(AssetHandle handle, const AssetMetadata& metadata)
	{
		Ref<Asset> prefabEntity = CreateRef<Prefab>();
		PrefabSerializer serializer = PrefabSerializer();
		if (serializer.TryLoadData(metadata, prefabEntity))
		{
			prefabEntity->Handle = handle;
			return As<Prefab>(prefabEntity);
		}
		
		return nullptr;
	}

}
