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

		// TODO Remove once asset pack properly implemented
		const AssetMetadata& GetAssetMetadata(AssetHandle handle) const;

		const AssetPak& GetAssets() const override { return m_AssetPak; }

	private:
		AssetMap m_LoadedAssets;
		AssetPak m_AssetPak;
	};
}
