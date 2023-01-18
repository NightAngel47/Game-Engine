#include "enginepch.h"
#include "Engine/Scene/PrefabSerializer.h"

#include "Engine/Scene/EntitySerializer.h"

namespace Engine
{
	void PrefabSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap; // Prefab
		out << YAML::Key << "Prefab" << YAML::Value << m_Entity.GetName();
		ENGINE_CORE_TRACE("Serializing prefab '{0}'", m_Entity.GetName());
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		EntitySerializer entitySerializer(m_Entity, m_Scene);
		entitySerializer.Serialize(out);
		out << YAML::EndSeq;
		out << YAML::EndMap; // Prefab

		std::ofstream fout(Project::GetAssetFileSystemPath(filepath).string());
		fout << out.c_str();
	}

	Entity PrefabSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(Project::GetAssetFileSystemPath(filepath).string());
		}
		catch (YAML::ParserException e)
		{
			ENGINE_CORE_ERROR("Failed to load .prefab file '{0}'\n {1}", filepath, e.what());
			return {};
		}

		if (!data["Prefab"])
			return {};

		std::string name = data["Prefab"].as<std::string>();
		ENGINE_CORE_TRACE("Deserializing prefab '{0}'", name);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				EntitySerializer entitySerializer({}, m_Scene);
				entitySerializer.Deserialize(entity);
			}
		}
	}
}
