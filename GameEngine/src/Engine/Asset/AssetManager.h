#pragma once
#include "Engine/Asset/Assets.h"

namespace Engine
{
	class AssetManager
	{
	public:
		static void Init();
		static void Shutdown();

		template <typename T>
		static Ref<T> GetAsset(AssetHandle assetID);
		template <typename T>
		static Ref<T> GetAsset(const std::filesystem::path& path);

		static AssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& path);

		template<typename T>
		static void SaveAsset(const AssetMetadata& metadata, const Ref<T>& asset);
	};

	namespace Utils
	{
		inline AssetType AssetTypeFromString(const std::string& assetType)
		{
			if (assetType == "None")			return AssetType::None;
			if (assetType == "Scene")			return AssetType::Scene;
			if (assetType == "Texture")			return AssetType::Texture;

			ENGINE_CORE_ASSERT(false, "Unknown Asset Type");
			return AssetType::None;
		}

		inline const char* AssetTypeToString(AssetType assetType)
		{
			switch (assetType)
			{
			case Engine::AssetType::None:		return "None";
			case Engine::AssetType::Scene:		return "Scene";
			case Engine::AssetType::Texture:	return "Texture";
			}

			ENGINE_CORE_ASSERT(false, "Unknown Asset Type");
			return "None";
		}
	}
}
