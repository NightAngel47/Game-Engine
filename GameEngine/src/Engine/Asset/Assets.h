#pragma once
#include "Engine/Core/UUID.h"

namespace Engine
{
	enum class AssetType : uint16_t
	{
		None = 0,
		Scene,
		Texture2D,
		Prefab,
		AudioClip,
		ScriptFile
	};

	using AssetHandle = UUID;

	struct Asset
	{
		AssetHandle Handle = AssetHandle::INVALID();

		// To be provide per type
		// static AssetType GetStaticType() { return AssetType::None; }
		virtual AssetType GetAssetType() const = 0;
	};

	struct AssetMetadata
	{
		AssetType Type = AssetType::None;
		std::filesystem::path Path;

		operator bool() const { return Type != AssetType::None; }
	};

	namespace Utils
	{
		inline AssetType AssetTypeFromString(const std::string& assetType)
		{
			if (assetType == "None")			return AssetType::None;
			if (assetType == "Scene")			return AssetType::Scene;
			if (assetType == "Texture2D")		return AssetType::Texture2D;
			if (assetType == "Prefab")			return AssetType::Prefab;
			if (assetType == "AudioClip")		return AssetType::AudioClip;

			ENGINE_CORE_ASSERT(false, "Unknown Asset Type");
			return AssetType::None;
		}

		inline const char* AssetTypeToString(AssetType assetType)
		{
			switch (assetType)
			{
			case Engine::AssetType::None:		return "None";
			case Engine::AssetType::Scene:		return "Scene";
			case Engine::AssetType::Texture2D:	return "Texture2D";
			case Engine::AssetType::Prefab:		return "Prefab";
			case Engine::AssetType::AudioClip:	return "AudioClip";
			}

			ENGINE_CORE_ASSERT(false, "Unknown Asset Type");
			return "None";
		}
	}
}
