#pragma once
#include "Engine/Asset/AssetManagerBase.h"

namespace Engine
{

	class RuntimeAssetManager : public AssetManagerBase
	{
	public:
		RuntimeAssetManager();
		
		Ref<Asset> GetAsset(AssetHandle handle) override;

		bool IsAssetHandleValid(AssetHandle handle) const override;
		bool IsAssetLoaded(AssetHandle handle) const override;

		const AssetMap GetAssetsOfType(AssetType type) const override;

		const uint32_t GetNumberOfAssetsInAssetPak() const { return m_AssetPak.size(); }

	private:
		AssetMap m_LoadedAssets;
		AssetPak m_AssetPak;
	};
}
