#pragma once
#include "Engine/Asset/Assets.h"
#include "Engine/Renderer/Texture.h"

namespace Engine
{
	class TextureImporter
	{
	public:
		static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Texture2D> ImportTexture2DFromPak(AssetHandle handle, const PakAssetEntry& pakEntry);
		static Ref<Texture2D> LoadTexture2D(const std::filesystem::path& filepath);
		static Ref<Texture2D> LoadTexture2D(const Buffer buffer);

		static void SaveTexture2D(const AssetMetadata& metadata, const Ref<Asset>& asset);
	};
}
