#pragma once
#include "Engine/Asset/AssetManagerBase.h"

namespace Engine
{
	using AssetRegistry = std::unordered_map<AssetHandle, AssetMetadata>; // TODO asset pack

	class RuntimeAssetManager : public AssetManagerBase
	{
	public:

	};
}
