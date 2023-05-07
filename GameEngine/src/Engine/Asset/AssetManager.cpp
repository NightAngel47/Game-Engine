#include "enginepch.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetImporter.h"
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

	template <typename T>
	Ref<T> AssetManager::GetAsset(const std::filesystem::path& path)
	{
		AssetMetadata metadata = AssetMetadata();
		metadata.Path = path;

		Ref<T> asset = CreateRef<T>();

		auto& assetPaths = s_AssetManagerData->AssetPaths;
		if (assetPaths.find(metadata.Path) == assetPaths.end())
		{
			// load asset from disk
			auto& serializers = s_AssetManagerData->Importer->GetAssetSerializerMap();
			if (serializers.at(asset->GetStaticType())->TryLoadData(metadata, As<Asset>(asset)))
			{
				// success
			}

			// else
			// create a new asset

			s_AssetManagerData->AssetsLoaded[metadata.Handle] = As<T>(asset);
			assetPaths[metadata.Path] = metadata.Handle;
		}
		else
		{
			// load asset from memory
			asset = As<T>(s_AssetManagerData->AssetsLoaded.at(assetPaths.at(path)));
		}

		return asset;
	}

	template <typename T>
	Ref<T> AssetManager::GetAsset(AssetHandle assetID)
	{
		AssetMetadata metadata = AssetMetadata();
		metadata.Handle = assetID;

		Ref<T> asset = CreateRef<T>();

		auto& assetsLoaded = s_AssetManagerData->AssetsLoaded;
		if (assetsLoaded.find(metadata.Handle) == assetsLoaded.end())
		{
			// load asset from disk
			auto& serializers = s_AssetManagerData->Importer->GetAssetSerializerMap();
			if (serializers.at(asset->GetStaticType())->TryLoadData(metadata, As<Asset>(asset)))
			{
				// success
			}

			// else
			// create a new asset

			asset->Handle = metadata.Handle;
			s_AssetManagerData->AssetPaths[metadata.Path] = metadata.Handle;
			assetsLoaded[metadata.Handle] = As<T>(asset);
		}
		else
		{
			// load asset from memory
			asset = As<T>(assetsLoaded[assetID]);
		}

		return asset;
	}

	template <typename T>
	void AssetManager::SaveAsset(const AssetMetadata& metadata, const Ref<T>& asset)
	{
		// save to disk
		auto& serializers = s_AssetManagerData->Importer->GetAssetSerializerMap();
		serializers.at(asset->GetStaticType())->Serialize(metadata, asset);

		// load copy to memory
		s_AssetManagerData->AssetPaths[metadata.Path] = metadata.Handle;
		s_AssetManagerData->AssetsLoaded[metadata.Handle] = asset;
	}

	AssetHandle AssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& path)
	{
		return s_AssetManagerData->AssetPaths.at(path);
	}


	const std::unordered_map<std::filesystem::path, Engine::AssetHandle> AssetManager::GetAssetPaths()
	{
		return s_AssetManagerData->AssetPaths;
	}

	template<>
	Ref<Scene> AssetManager::GetAsset<Scene>(const std::filesystem::path& path)
	{
		AssetMetadata metadata = AssetMetadata();
		metadata.Path = path;

		Ref<Scene> asset = CreateRef<Scene>();

		auto& assetPaths = s_AssetManagerData->AssetPaths;
		if (assetPaths.find(metadata.Path) == assetPaths.end())
		{
			// load scene from disk
			auto& serializers = s_AssetManagerData->Importer->GetAssetSerializerMap();
			if (serializers.at(asset->GetStaticType())->TryLoadData(metadata, As<Asset>(asset)))
			{
				// success
			}
			
			// else
			// create a new scene

			s_AssetManagerData->AssetsLoaded[metadata.Handle] = As<Scene>(asset);
			assetPaths[metadata.Path] = metadata.Handle;
		}
		else
		{
			// load scene from memory
			asset = As<Scene>(s_AssetManagerData->AssetsLoaded.at(assetPaths.at(path)));
		}

		return asset;
	}

	template<>
	Ref<Scene> AssetManager::GetAsset<Scene>(AssetHandle assetID)
	{
		AssetMetadata metadata = AssetMetadata();
		metadata.Handle = assetID;

		Ref<Scene> asset = CreateRef<Scene>();

		auto& assetsLoaded = s_AssetManagerData->AssetsLoaded;
		if (assetsLoaded.find(metadata.Handle) == assetsLoaded.end())
		{
			// load scene from disk
			auto& serializers = s_AssetManagerData->Importer->GetAssetSerializerMap();
			if (serializers.at(asset->GetStaticType())->TryLoadData(metadata, As<Asset>(asset)))
			{
				// success
			}

			// else
			// create a new scene

			asset->Handle = metadata.Handle;
			s_AssetManagerData->AssetPaths[metadata.Path] = metadata.Handle;
			assetsLoaded[metadata.Handle] = As<Scene>(asset);
		}
		else
		{
			// load scene from memory
			asset = As<Scene>(assetsLoaded[assetID]);
		}

		return asset;
	}

	template<>
	void AssetManager::SaveAsset<Scene>(const AssetMetadata& metadata, const Ref<Scene>& asset)
	{
		// save to disk
		auto& serializers = s_AssetManagerData->Importer->GetAssetSerializerMap();
		serializers.at(asset->GetAssetType())->Serialize(metadata, asset);

		// load copy to memory
		s_AssetManagerData->AssetPaths[metadata.Path] = metadata.Handle;
		s_AssetManagerData->AssetsLoaded[metadata.Handle] = asset;
	}
}
