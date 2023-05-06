#include "enginepch.h"
#include "Engine/Asset/AssetImporter.h"
#include "Engine/Asset/Assets.h"

#include "Engine/Scene/SceneSerializer.h"

namespace Engine
{
	static std::unordered_map<AssetType, Ref<AssetSerializer>> s_Serializers;

	void AssetImporter::Init()
	{
		s_Serializers[AssetType::Scene] = CreateScope<SceneSerializer>();
	}

	void AssetImporter::Shutdown()
	{

	}

	AssetSerializerMap& AssetImporter::GetAssetSerializerMap()
	{
		return s_Serializers;
	}
}
