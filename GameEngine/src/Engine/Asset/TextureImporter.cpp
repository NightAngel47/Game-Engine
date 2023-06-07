#include "enginepch.h"
#include "Engine/Asset/TextureImporter.h"
#include "Engine/Project/Project.h"

#include <stb_image.h>

namespace Engine
{
	Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata, bool isResource)
	{
		ENGINE_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		Buffer data;
		{
			ENGINE_PROFILE_SCOPE("stbi_load - TextureImporter::ImportTexture2D");
			std::string assetPath = isResource ? metadata.Path.string() : Project::GetAssetFileSystemPath(metadata.Path).string();
			data.Data = stbi_load(assetPath.c_str(), &width, &height, &channels, 0);
		}

		if (data.Data == nullptr)
		{
			ENGINE_CORE_ERROR("Failed to load image!");
			return nullptr;
		}

		// TODO: might break later
		data.Size = width * height * channels;

		TextureSpecification spec;
		spec.Width = width;
		spec.Height = height;

		switch (channels)
		{
			case 3:
				spec.Format = ImageFormat::RGB8;
				break;
			case 4:
				spec.Format = ImageFormat::RGBA8;
				break;
		}

		Ref<Texture2D> texture = Texture2D::Create(spec, data);
		texture->Handle = handle;
		data.Release();
		return texture;
	}
}
