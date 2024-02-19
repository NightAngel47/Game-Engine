#pragma once
#include "Engine/Asset/AssetManagerBase.h"

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
