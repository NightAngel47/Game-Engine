#include "enginepch.h"
#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Scene/EntitySerializer.h"

namespace Engine
{
	void SceneSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap; // Scene
		out << YAML::Key << "Scene" << YAML::Value << m_Scene->m_Name;
		ENGINE_CORE_TRACE("Serializing scene '{0}'", m_Scene->m_Name);
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene.get() };
			if(!entity) return;

			EntitySerializer entitySerializer(entity, m_Scene);
			entitySerializer.Serialize(out);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap; // Scene

		std::ofstream fout(Project::GetAssetFileSystemPath(filepath).string());
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::filesystem::path& filepath)
	{
		// Not implemented yet
		ENGINE_CORE_ASSERT(false, "Not implemented yet");
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(Project::GetAssetFileSystemPath(filepath).string());
		}
		catch (YAML::ParserException e)
		{
			ENGINE_CORE_ERROR("Failed to load .scene file '{0}'\n {1}", filepath, e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		m_Scene->m_Name = data["Scene"].as<std::string>();
		ENGINE_CORE_TRACE("Deserializing scene '{0}'", m_Scene->m_Name);

		auto entities = data["Entities"];
		if(entities)
		{
			for (auto entity : entities)
			{
				EntitySerializer entitySerializer({}, m_Scene);
				entitySerializer.Deserialize(entity);
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::filesystem::path& filepath)
	{
		// Not implemented yet
		ENGINE_CORE_ASSERT(false, "Not implemented yet");

		return false;
	}
}
