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

		std::ofstream fout(Project::GetAssetRegistryPath().string());
		fout << out.c_str();
	}

	bool AssetRegistrySerializer::TryLoadData(AssetRegistry& assetRegistry)
	{
		std::filesystem::path assetRegistryPath = Project::GetAssetRegistryPath();

		if (!std::filesystem::exists(assetRegistryPath))
			return false;

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(assetRegistryPath.string());
		}
		catch (YAML::ParserException e)
		{
			ENGINE_CORE_ERROR("Failed to load asset registry file '{0}'\n {1}", assetRegistryPath.string(), e.what());
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
}
