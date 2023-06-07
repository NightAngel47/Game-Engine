#include "enginepch.h"
#include "Engine/Renderer/TextureSerializer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Project/Project.h"

namespace Engine
{
	class OpenGLTexture2D;

	void TextureSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		// TODO - is this needed?
	}
	
	bool TextureSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		if (metadata.Path.empty())
			return false;

		ENGINE_CORE_WARN("Use Count: {0}", asset.use_count());

		//asset = Texture2D::Create(Project::GetAssetFileSystemPath(metadata.Path));

		ENGINE_CORE_WARN("Use Count: {0}", asset.use_count());

		return true;
	}
}

