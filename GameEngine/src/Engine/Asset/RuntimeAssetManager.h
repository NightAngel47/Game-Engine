#pragma once
#include "Engine/Asset/AssetManagerBase.h"

#include <map>

namespace Engine
{
	using AssetPack = std::map<AssetHandle, AssetMetadata>; // TODO make proper asset pack (using asset registry atm)

	class RuntimeAssetManager : public AssetManagerBase
	{
	public:
		RuntimeAssetManager();

		Ref<Asset> GetAsset(AssetHandle handle) override;

		bool IsAssetHandleValid(AssetHandle handle) const override;
		bool IsAssetLoaded(AssetHandle handle) const override;

		// TODO Remove once asset pack properly implemented
		const AssetMetadata& GetAssetMetadata(AssetHandle handle) const;

		const AssetPack& GetAssetPack() const { return m_AssetPack; }

	private:
		AssetMap m_LoadedAssets;
		AssetPack m_AssetPack;
	};
}
