#include "enginepch.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{
	struct AssetManagerData
	{
		std::unordered_map<std::filesystem::path, AssetHandle> AssetPaths;
		std::unordered_map<AssetHandle, Ref<Asset>> AssetsLoaded;

		AssetImporter* Importer = nullptr;
	};

	static AssetManagerData* s_AssetManagerData = nullptr;

	void AssetManager::Init()
	{
		s_AssetManagerData = new AssetManagerData();
		s_AssetManagerData->Importer = new AssetImporter();
		s_AssetManagerData->Importer->Init();
	}

	void AssetManager::Shutdown()
	{
		delete s_AssetManagerData;
	}

	AssetHandle AssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& path)
	{
		return s_AssetManagerData->AssetPaths.at(path);
	}

	AssetPathsMap& AssetManager::GetAssetPathsMap()
	{
		return s_AssetManagerData->AssetPaths;
	}

	AssetsLoadedMap& AssetManager::GetAssetsLoadedMap()
	{
		return s_AssetManagerData->AssetsLoaded;
	}

	AssetImporter& AssetManager::GetAssetImporter()
	{
		return *s_AssetManagerData->Importer;
	}
}
