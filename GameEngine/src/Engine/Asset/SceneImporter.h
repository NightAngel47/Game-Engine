#pragma once
#include "Engine/Asset/Assets.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{
	class SceneImporter
	{
	public:
		static Ref<Scene> ImportScene(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Scene> ImportSceneFromPak(AssetHandle handle, const PakAssetEntry& pakEntry);
		static void SaveScene(const AssetMetadata& metadata, const Ref<Asset>& asset);
	};
}
