#pragma once
#include "Engine/Asset/AssetImporter.h"

namespace Engine
{
	class PrefabSerializer : public AssetSerializer
	{
	public:
		PrefabSerializer() = default;

		//void Serialize(const std::filesystem::path& filepath, Entity entity, const Ref<Scene>& scene);
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;

		//void Deserialize(const std::filesystem::path& filepath, Entity entity, Ref<Scene>& scene);
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};
}
