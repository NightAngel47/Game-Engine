#include "enginepch.h"
#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/EntitySerializer.h"

namespace Engine
{
	void SceneSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		YAML::Emitter out;
		out << YAML::BeginMap; // Scene
		Ref<Scene> scene = As<Scene>(asset);
		out << YAML::Key << "Scene" << YAML::Value << scene->m_Name;
		ENGINE_CORE_TRACE("Serializing scene '{0}'", scene->m_Name);
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, scene.get() };
			if(!entity || entity.GetComponent<RelationshipComponent>().Parent.IsValid())
				return;

			EntitySerializer entitySerializer = EntitySerializer();
			entitySerializer.Serialize(out, entity, scene);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap; // Scene
		
		std::ofstream fout(Project::GetActiveAssetFileSystemPath(metadata.Path).string());
		fout << out.c_str();
	}

	bool SceneSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		if (metadata.Path.empty())
			return false;

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(Project::GetActiveAssetFileSystemPath(metadata.Path).string());
		}
		catch (YAML::ParserException e)
		{
			ENGINE_CORE_ERROR("Failed to load .scene file '{0}'\n {1}", metadata.Path, e.what());
			return false;
		}
		
		if (!data["Scene"])
			return false;
		
		std::string sceneName = data["Scene"].as<std::string>();
		ENGINE_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		asset = CreateRef<Scene>();
		Ref<Scene> scene = As<Scene>(asset);
		scene->SetSceneName(sceneName);
		
		auto entities = data["Entities"];
		if(entities)
		{
			for (auto entity : entities)
			{
				Entity thisEntity{ {}, scene.get() };
				EntitySerializer entitySerializer = EntitySerializer();
				entitySerializer.Deserialize(entity, thisEntity, scene);
			}
		}
		
		return true;
	}

	bool SceneSerializer::TryLoadData(const PakAssetEntry& pakEntry, Ref<Asset>& asset) const
	{
		std::filesystem::path assetPakPath = Project::GetActiveAssetPakPath();
		std::ifstream fileStream(assetPakPath, std::ios::binary);
		if (fileStream.fail())
		{
			ENGINE_CORE_ERROR("Failed to open the file!");
			return false;
		}

		uint32_t numberOfEntries = Project::GetActive()->GetRuntimeAssetManager()->GetNumberOfAssetsInAssetPak();
		fileStream.seekg(sizeof(PakHeader) + sizeof(PakAssetEntry) * numberOfEntries + pakEntry.OffSet);

		std::vector<char> fileData;
		fileData.resize(pakEntry.UncompressedSize); //TODO change when compression
		fileStream.read(fileData.data(), pakEntry.UncompressedSize);
		fileData.insert(fileData.end(), '\n');

		YAML::Node data;
		try
		{
			data = YAML::Load(reinterpret_cast<const char*>(fileData.data()));
		}
		catch (YAML::ParserException e)
		{
			ENGINE_CORE_ERROR("Failed to load Scene with AssetHandle: '{0}'\n {1}", pakEntry.Handle, e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		ENGINE_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		asset = CreateRef<Scene>();
		Ref<Scene> scene = As<Scene>(asset);
		scene->SetSceneName(sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				Entity thisEntity{ {}, scene.get() };
				EntitySerializer entitySerializer = EntitySerializer();
				entitySerializer.Deserialize(entity, thisEntity, scene);
			}
		}

		return true;
	}

}
