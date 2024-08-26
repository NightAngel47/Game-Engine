#pragma once
#include "Engine/Asset/AssetImporter.h"

namespace Engine
{
	class TextureSerializer : public AssetSerializer
	{
	public:

		//TODO: Remove? Reevaluate asset pipeline
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		//TODO: Remove? Reevaluate asset pipeline
		virtual const std::vector<char> SerializeForStream(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
		virtual bool TryLoadData(const PakAssetEntry& pakEntry, Ref<Asset>& asset) const override;


	};
}
