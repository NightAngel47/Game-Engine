#pragma once
#include "Engine/Asset/EditorAssetManager.h"

#include <filesystem>

namespace Engine
{
	class AssetRegistrySerializer
	{
	public:
		AssetRegistrySerializer() = default;

		void Serialize(const AssetRegistry& assetRegistry);
		bool TryLoadData(AssetRegistry& assetRegistry);
	};
}
