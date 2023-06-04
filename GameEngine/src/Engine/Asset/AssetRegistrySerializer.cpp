#include "enginepch.h"
#include "Engine/Asset/AssetRegistrySerializer.h"
#include "Engine/Project/Project.h"

#include <yaml-cpp/yaml.h>

namespace Engine
{
	void AssetRegistrySerializer::Serialize(const AssetRegistry& assetRegistry)
	{
		YAML::Emitter out;
		out << YAML::BeginMap; // AssetRegistry
		out << YAML::Key << "Assets" << YAML::Value << YAML::BeginSeq;

		for (const auto& [handle, metadata] : assetRegistry)
		{
			out << YAML::BeginMap; // Asset
			
			out << YAML::Key << "Handle" << YAML::Value << handle;
			out << YAML::Key << "FilePath" << YAML::Value << metadata.Path.string();
			out << YAML::Key << "Type" << YAML::Value << Utils::AssetTypeToString(metadata.Type);
			
			out << YAML::EndMap; // Asset
		}

		out << YAML::EndSeq;

		std::ofstream fout(GetAssetRegistryPath().string());
		fout << out.c_str();
	}

	bool AssetRegistrySerializer::TryLoadData(AssetRegistry& assetRegistry)
	{
		if (!std::filesystem::exists(GetAssetRegistryPath()))
			return false;

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(GetAssetRegistryPath().string());
		}
		catch (YAML::ParserException e)
		{
			ENGINE_CORE_ERROR("Failed to load asset registry file '{0}'\n {1}", GetAssetRegistryPath().string(), e.what());
			return false;
		}

		auto assets = data["Assets"];
		if (assets)
		{
			for (auto asset : assets)
			{
				AssetHandle handle = asset["Handle"].as<uint64_t>();

				AssetMetadata metadata = AssetMetadata();
				metadata.Path = asset["FilePath"].as<std::string>();
				std::string typeString = asset["Type"].as<std::string>();
				metadata.Type = Utils::AssetTypeFromString(typeString);

				assetRegistry[handle] = metadata;
			}
		}

		return true;
	}


	const std::filesystem::path AssetRegistrySerializer::GetAssetRegistryPath()
	{
		std::filesystem::path assetRegistryPath = "AssetRegistry.areg";
		return Project::GetProjectDirectory() / assetRegistryPath;
	}

}
