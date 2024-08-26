#pragma once
#include "Engine/Asset/Assets.h"
#include "Engine/Project/Project.h"

namespace Engine
{
	class AssetManager
	{
	public:
		template<typename T> 
		static Ref<T> GetAsset(AssetHandle handle)
		{
			if (!handle.IsValid())
				return nullptr;

			Ref<Asset> asset = Project::GetActive()->GetAssetManager()->GetAsset(handle);
			return As<T>(asset);
		}

		static bool IsAssetHandleValid(AssetHandle handle)
		{
			return handle.IsValid() && Project::GetActive()->GetAssetManager()->IsAssetHandleValid(handle);
		}

		static bool IsAssetLoaded(AssetHandle handle)
		{
			return handle.IsValid() && Project::GetActive()->GetAssetManager()->IsAssetLoaded(handle);
		}

		static const AssetMap GetAssetsOfType(AssetType type)
		{
			return Project::GetActive()->GetAssetManager()->GetAssetsOfType(type);
		}
	};
}
