#pragma once
#include "Engine/Asset/Assets.h"
#include "Engine/Asset/AssetImporter.h"

namespace Engine
{
	using AssetPathsMap = std::unordered_map<std::filesystem::path, AssetHandle>;
	using AssetsLoadedMap = std::unordered_map<AssetHandle, Ref<Asset>>;

	class AssetManager
	{
	public:
		static void Init();
		static void Shutdown();

		static AssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& path);

		static AssetPathsMap& GetAssetPathsMap();
		static AssetsLoadedMap& GetAssetsLoadedMap();
		static AssetImporter& GetAssetImporter();

		template <typename T>
		static Ref<T> GetAsset(AssetHandle assetID)
		{
			AssetMetadata metadata = AssetMetadata();
			metadata.Handle = assetID;

			Ref<T> asset = CreateRef<T>();

			AssetsLoadedMap& assetsLoaded = GetAssetsLoadedMap();
			if (assetsLoaded.find(metadata.Handle) == assetsLoaded.end())
			{
				// load asset from disk
				AssetImporter& importer = GetAssetImporter();
				auto& serializers = importer.GetAssetSerializerMap();
				if (serializers.at(asset->GetStaticType())->TryLoadData(metadata, As<Asset>(asset)))
				{
					// success
				}

				// else
				// create a new asset

				asset->Handle = metadata.Handle;

				AssetPathsMap& assetPaths = GetAssetPathsMap();
				assetPaths[metadata.Path] = metadata.Handle;
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
		static Ref<T> GetAsset(const std::filesystem::path& path)
		{
			AssetMetadata metadata = AssetMetadata();
			metadata.Path = path;

			Ref<T> asset = CreateRef<T>();

			AssetPathsMap& assetPaths = GetAssetPathsMap();
			if (assetPaths.find(metadata.Path) == assetPaths.end())
			{
				// load asset from disk
				AssetImporter& importer = GetAssetImporter();
				auto& serializers = importer.GetAssetSerializerMap();
				if (serializers.at(asset->GetStaticType())->TryLoadData(metadata, As<Asset>(asset)))
				{
					// success
				}

				// else
				// create a new asset

				AssetsLoadedMap& assetsLoaded = GetAssetsLoadedMap();
				assetsLoaded[metadata.Handle] = As<T>(asset);
				assetPaths[metadata.Path] = metadata.Handle;
			}
			else
			{
				// load asset from memory
				AssetsLoadedMap& assetsLoaded = GetAssetsLoadedMap();
				asset = As<T>(assetsLoaded.at(assetPaths.at(path)));
			}

			return asset;
		}
		
		template<typename T>
		static void SaveAsset(const AssetMetadata& metadata, const Ref<T>& asset)
		{
			// save to disk
			AssetImporter& importer = GetAssetImporter();
			auto& serializers = importer.GetAssetSerializerMap();
			serializers.at(asset->GetStaticType())->Serialize(metadata, asset);

			// load copy to memory
			AssetPathsMap& assetPaths = GetAssetPathsMap();
			assetPaths[metadata.Path] = metadata.Handle;

			AssetsLoadedMap& assetsLoaded = GetAssetsLoadedMap();
			assetsLoaded[metadata.Handle] = asset;
		}
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
