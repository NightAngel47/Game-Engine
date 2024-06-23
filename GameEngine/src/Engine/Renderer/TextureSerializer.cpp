#include "enginepch.h"
#include "Engine/Renderer/TextureSerializer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Project/Project.h"

namespace Engine
{
	class OpenGLTexture2D;

	void TextureSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{

	}
	
	bool TextureSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		if (metadata.Path.empty())
			return false;

		// TODO remove?

		return true;
	}

	bool TextureSerializer::TryLoadData(const PakAssetEntry& pakEntry, Ref<Asset>& asset) const
	{
		return true;
	}
}

