#include "enginepch.h"
#include "Engine/Scene/PrefabSerializer.h"
#include "Engine/Scene/Prefab.h"

#include "Engine/Scene/EntitySerializer.h"
#include "Engine/Scene/SceneManager.h"

namespace Engine
{
	struct PrefabData
	{
		UUID Parent = UUID::INVALID();
	};

	void PrefabSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		std::vector<char> serializedAsset = SerializeForStream(metadata, asset);

		std::ofstream fout(Project::GetActiveAssetFileSystemPath(metadata.Path).string());
		fout << serializedAsset.data() << std::endl;
	}

	const std::vector<char> PrefabSerializer::SerializeForStream(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		YAML::Emitter out;
		out << YAML::BeginDoc; // Prefab
		out << YAML::BeginMap; // Prefab
		Ref<Prefab> prefab = As<Prefab>(asset);
		Entity entity = prefab->m_PrefabEntity;
		out << YAML::Key << "Prefab" << YAML::Value << entity.GetName();
		ENGINE_CORE_TRACE("Serializing prefab '{0}'", entity.GetName());
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		EntitySerializer entitySerializer = EntitySerializer();
		entitySerializer.Serialize(out, entity, prefab->m_PrefabScene);
		out << YAML::EndSeq;
		out << YAML::EndMap; // Prefab
		out << YAML::EndDoc; // Prefab

		std::vector<char> buffer;
		buffer.resize(out.size());
		for (int i = 0; i < out.size(); i++)
			buffer[i] = out.c_str()[i];
		return buffer;
	}

	bool PrefabSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
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
			ENGINE_CORE_ERROR("Failed to load .prefab file '{0}'\n {1}", metadata.Path, e.what());
			return false;
		}

		if (!data["Prefab"])
			return false;

		std::string name = data["Prefab"].as<std::string>();
		ENGINE_CORE_TRACE("Deserializing prefab '{0}'", name);

		asset = CreateRef<Prefab>();
		Ref<Prefab> prefab = As<Prefab>(asset);

		auto entities = data["Entities"];
		if (entities)
		{
			prefab->m_PrefabScene = CreateRef<Scene>();
			Entity thisEntity{ {}, prefab->m_PrefabScene.get() };
			prefab->m_PrefabEntity = thisEntity;

			for (auto entityOut : entities)
			{
				EntitySerializer entitySerializer = EntitySerializer();
				entitySerializer.Deserialize(entityOut, prefab->m_PrefabEntity, prefab->m_PrefabScene, true);
			}
		}

		return true;
	}

	bool PrefabSerializer::TryLoadData(const PakAssetEntry& pakEntry, Ref<Asset>& asset) const
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

		YAML::Node data;
		try
		{
			data = YAML::Load(fileData.data());
		}
		catch (YAML::ParserException e)
		{
			ENGINE_CORE_ERROR("Failed to load Prefab with AssetHandle: '{0}'\n {1}", pakEntry.Handle, e.what());
			return false;
		}

		if (!data["Prefab"])
			return false;

		std::string name = data["Prefab"].as<std::string>();
		ENGINE_CORE_TRACE("Deserializing prefab '{0}'", name);

		asset = CreateRef<Prefab>();
		Ref<Prefab> prefab = As<Prefab>(asset);

		auto entities = data["Entities"];
		if (entities)
		{
			prefab->m_PrefabScene = CreateRef<Scene>();
			Entity thisEntity{ {}, prefab->m_PrefabScene.get() };
			prefab->m_PrefabEntity = thisEntity;

			for (auto entityOut : entities)
			{
				EntitySerializer entitySerializer = EntitySerializer();
				entitySerializer.Deserialize(entityOut, prefab->m_PrefabEntity, prefab->m_PrefabScene, true);
			}
		}

		return true;
	}

}
