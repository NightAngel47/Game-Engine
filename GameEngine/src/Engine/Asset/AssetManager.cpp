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
		Ref<T> asset = CreateRef<T>();

		auto& assetPaths = s_AssetManagerData->AssetPaths;
		if (assetPaths.find(path) == assetPaths.end())
		{
			auto& serializers = s_AssetManagerData->Importer->GetAssetSerializerMap();

			AssetMetadata metadata = AssetMetadata();
			metadata.Path = path;
			
			if (serializers.at(asset->GetStaticType())->TryLoadData(metadata, As<Asset>(asset)))
			{
				assetPaths[metadata.Path] = metadata.Handle;
				s_AssetManagerData->AssetsLoaded[metadata.Handle] = asset;
			}
		}
		else
		{
			AssetHandle assetID = assetPaths[path];
			Ref<Asset> loadedAsset = s_AssetManagerData->AssetsLoaded[assetID];
			asset = As<T>(loadedAsset);
		}

		return asset;
	}

	template <typename T>
	Ref<T> AssetManager::GetAsset(AssetHandle assetID)
	{
		Ref<T> asset = CreateRef<T>();

		auto& assetsLoaded = s_AssetManagerData->AssetsLoaded;
		if (assetsLoaded.find(assetID) == assetsLoaded.end())
		{
			auto& serializers = s_AssetManagerData->Importer->GetAssetSerializerMap();

			AssetMetadata metadata = AssetMetadata();
			metadata.Handle = assetID;

			if (serializers.at(asset->GetStaticType())->TryLoadData(metadata, As<Asset>(asset)))
			{
				s_AssetManagerData->AssetPaths[metadata.Path] = assetID;
				assetsLoaded[metadata.Handle] = asset;
			}
		}
		else
		{
			Ref<Asset> loadedAsset = assetsLoaded[assetID];
			asset = As<T>(loadedAsset);
		}

		return asset;
	}

	AssetHandle AssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& path)
	{
		return s_AssetManagerData->AssetPaths[path];
	}

	template <typename T>
	void AssetManager::SaveAsset(const AssetMetadata& metadata, const Ref<T>& asset)
	{
		auto& serializers = s_AssetManagerData->Importer->GetAssetSerializerMap();
		serializers.at(asset->GetStaticType())->Serialize(metadata, asset);
	}

	template<>
	Ref<Scene> AssetManager::GetAsset<Scene>(const std::filesystem::path& path)
	{
		AssetMetadata metadata = AssetMetadata();
		metadata.Path = path;
		Ref<Scene> asset = As<Scene>(s_AssetManagerData->AssetsLoaded[metadata.Handle]);
		asset = CreateRef<Scene>();
		std::cout << "Asset Use Count " << asset.use_count() << std::endl;

		auto& assetPaths = s_AssetManagerData->AssetPaths;
		if (assetPaths.find(path) == assetPaths.end())
		{
			auto& serializers = s_AssetManagerData->Importer->GetAssetSerializerMap();
			if (serializers.at(asset->GetStaticType())->TryLoadData(metadata, As<Asset>(asset)))
			{
				std::cout << "Asset Use Count " << asset.use_count() << std::endl;
				assetPaths[metadata.Path] = metadata.Handle;
				//s_AssetManagerData->AssetsLoaded[metadata.Handle] = asset;
			}
		}
		else
		{
			AssetHandle assetID = assetPaths[path];
			Ref<Asset> loadedAsset = s_AssetManagerData->AssetsLoaded[assetID];
			asset = As<Scene>(loadedAsset);
			std::cout << "Asset Use Count " << asset.use_count() << std::endl;
		}

		std::cout << "Asset Use Count " << asset.use_count() << std::endl;
		return asset;
	}

	template<>
	Ref<Scene> AssetManager::GetAsset<Scene>(AssetHandle assetID)
	{
		Ref<Scene> asset = CreateRef<Scene>();

		auto& assetsLoaded = s_AssetManagerData->AssetsLoaded;
		if (assetsLoaded.find(assetID) == assetsLoaded.end())
		{
			auto& serializers = s_AssetManagerData->Importer->GetAssetSerializerMap();

			AssetMetadata metadata = AssetMetadata();
			metadata.Handle = assetID;

			if (serializers.at(asset->GetStaticType())->TryLoadData(metadata, As<Asset>(asset)))
			{
				s_AssetManagerData->AssetPaths[metadata.Path] = assetID;
				assetsLoaded[metadata.Handle] = asset;
			}
		}
		else
		{
			Ref<Asset> loadedAsset = assetsLoaded[assetID];
			asset = As<Scene>(loadedAsset);
		}

		return asset;
	}

	template<>
	void AssetManager::SaveAsset<Scene>(const AssetMetadata& metadata, const Ref<Scene>& asset)
	{
		auto& serializers = s_AssetManagerData->Importer->GetAssetSerializerMap();
		serializers.at(asset->GetAssetType())->Serialize(metadata, asset);
	}
}
