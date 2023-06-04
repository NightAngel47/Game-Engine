#pragma once
#include "Engine/Asset/AssetManagerBase.h"

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
