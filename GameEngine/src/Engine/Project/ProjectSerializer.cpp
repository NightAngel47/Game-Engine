#include "enginepch.h"
#include "Engine/Project/ProjectSerializer.h"

#include <yaml-cpp/yaml.h>

namespace Engine
{
	void ProjectSerializer::Serialize(const std::filesystem::path& filepath)
	{
		const auto& config = m_Project->GetConfig();

		YAML::Emitter out;
		out << YAML::BeginMap; // Root
		out << YAML::Key << "Project" << YAML::Value;
		out << YAML::BeginMap; // Project

		out << YAML::Key << "Name" << YAML::Value << config.Name;
		ENGINE_CORE_TRACE("Serializing Project '{0}'", config.Name);
		out << YAML::Key << "StartScene" << YAML::Value << config.StartScene.string();
		out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
		out << YAML::Key << "ScriptModulePath" << YAML::Value << config.ScriptModulePath.string();

		out << YAML::EndMap; // Project
		out << YAML::EndMap; // Root

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		auto& config = m_Project->GetConfig();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			ENGINE_CORE_ERROR("Failed to load project file '{0}'\n {1}", filepath, e.what());
			return false;
		}

		auto projectNode = data["Project"];
		if (!projectNode)
			return false;

		config.Name = projectNode["Name"].as<std::string>();
		ENGINE_CORE_TRACE("Deserializing project '{0}'", config.Name);
		config.StartScene = projectNode["StartScene"].as<std::string>();
		config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
		config.ScriptModulePath = projectNode["ScriptModulePath"].as<std::string>();

		return true;
	}
}
