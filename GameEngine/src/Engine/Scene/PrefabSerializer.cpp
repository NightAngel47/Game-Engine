#include "enginepch.h"
#include "Engine/Scene/PrefabSerializer.h"

#include "Engine/Scene/EntitySerializer.h"

namespace Engine
{
	struct PrefabData
	{
		UUID Parent = UUID::INVALID();
	};

	void PrefabSerializer::Serialize(const std::filesystem::path& filepath, Entity entity, const Ref<Scene>& scene)
	{
		YAML::Emitter out;
		out << YAML::BeginMap; // Prefab
		out << YAML::Key << "Prefab" << YAML::Value << entity.GetName();
		ENGINE_CORE_TRACE("Serializing prefab '{0}'", entity.GetName());
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		EntitySerializer entitySerializer = EntitySerializer();
		entitySerializer.Serialize(out, entity, scene);
		out << YAML::EndSeq;
		out << YAML::EndMap; // Prefab

		std::ofstream fout(Project::GetAssetFileSystemPath(filepath).string());
		fout << out.c_str();
	}

	void PrefabSerializer::Deserialize(const std::filesystem::path& filepath, Entity entity, Ref<Scene>& scene)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(Project::GetAssetFileSystemPath(filepath).string());
		}
		catch (YAML::ParserException e)
		{
			ENGINE_CORE_ERROR("Failed to load .prefab file '{0}'\n {1}", filepath, e.what());
			return;
		}

		if (!data["Prefab"])
			return;

		std::string name = data["Prefab"].as<std::string>();
		ENGINE_CORE_TRACE("Deserializing prefab '{0}'", name);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entityOut : entities)
			{
				EntitySerializer entitySerializer = EntitySerializer();
				entitySerializer.Deserialize(entityOut, entity, scene, true);
			}
		}
	}
}
