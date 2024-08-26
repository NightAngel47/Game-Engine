#pragma once
#include "Engine/Asset/AssetManagerBase.h"

//https://simoncoenen.com/blog/programming/PakFiles

namespace Engine
{
	class AssetPakSerializer
	{
	public:
		AssetPakSerializer() = default;

		void Serialize(const AssetRegistry& assetRegistry);
		bool TryLoadData(AssetPak& assetPak);
	};
}
