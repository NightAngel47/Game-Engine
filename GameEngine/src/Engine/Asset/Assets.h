#pragma once
#include "Engine/Core/UUID.h"

namespace Engine
{
	enum class AssetType : uint16_t
	{
		None = 0,
		Scene = 1,
		Texture = 2,
	};

	struct AssetHandle
	{
		AssetHandle() = default;
		AssetHandle(UUID assetID)
			:AssetID(assetID) {}
		AssetHandle(const AssetHandle& other) = default;

		operator bool() const { return AssetID != UUID::INVALID(); }

		bool operator==(const AssetHandle other) const { return AssetID == other.AssetID; }


		UUID AssetID{};
	};

	struct Asset
	{
		Asset() = default;

		AssetHandle Handle;

		// To be provide per type
		static AssetType GetStaticType() { return AssetType::None; }
		virtual AssetType GetAssetType() const = 0;
	};

	struct AssetMetadata
	{
		AssetHandle Handle;
		std::filesystem::path Path;
	};
}

namespace std
{
	template <typename T> struct hash;
	
	template<>
	struct hash<Engine::AssetHandle>
	{
		std::size_t operator()(const Engine::AssetHandle& handle) const
		{
			return (uint64_t)handle.AssetID;
		}
	};
}
