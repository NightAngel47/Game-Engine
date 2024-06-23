#pragma once
#include "Engine/Asset/AssetImporter.h"

namespace Engine
{
	class SceneSerializer : public AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
		virtual bool TryLoadData(const PakAssetEntry& metadata, Ref<Asset>& asset) const override;
	};
}
