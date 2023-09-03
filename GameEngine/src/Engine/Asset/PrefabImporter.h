#pragma once
#include "Engine/Scene/Prefab.h"

namespace Engine
{
	class PrefabImporter
	{
	public:
		static Ref<Prefab> ImportPrefab(AssetHandle handle, const AssetMetadata& metadata);
	};
}
