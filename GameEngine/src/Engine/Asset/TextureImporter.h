#pragma once
#include "Engine/Asset/Assets.h"
#include "Engine/Renderer/Texture.h"

namespace Engine
{
	class TextureImporter
	{
	public:
		static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata, bool isResource = false);
	};
}
