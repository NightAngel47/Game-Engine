#pragma once
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/AssetImporter.h"

namespace Engine
{
	class SceneSerializer : public AssetSerializer
	{
	public:
		//void Serialize (const std::filesystem::path& filepath);
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		void SerializeRuntime(const std::filesystem::path& filepath);
		
		//bool Deserialize(const std::filesystem::path& filepath);
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset> asset) const override;
		bool DeserializeRuntime(const std::filesystem::path& filepath);
	};
}
