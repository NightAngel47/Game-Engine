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
			Ref<Asset> asset = Project::GetActive()->GetAssetManager()->GetAsset(handle);
			return As<T>(asset);
		}
	};
}
