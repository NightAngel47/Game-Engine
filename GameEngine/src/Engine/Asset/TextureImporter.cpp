#include "enginepch.h"
#include "Engine/Asset/TextureImporter.h"
#include "Engine/Project/Project.h"
#include "Engine/Renderer/TextureSerializer.h"

#include <stb_image.h>

namespace Engine
{
	Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
	{
		ENGINE_PROFILE_FUNCTION();

		Ref<Texture2D> texture = LoadTexture2D(Project::GetActiveAssetFileSystemPath(metadata.Path).string());
		texture->Handle = handle;
		return texture;
	}

	Ref<Texture2D> TextureImporter::ImportTexture2DFromPak(AssetHandle handle, const PakAssetEntry& pakEntry)
	{
		ENGINE_PROFILE_FUNCTION();

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

		Ref<Texture2D> texture = LoadTexture2D({fileData.data(), fileData.size()});
		texture->Handle = handle;
		return texture;
	}

	Ref<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& filepath)
	{
		ENGINE_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		Buffer data;
		{
			ENGINE_PROFILE_SCOPE("stbi_load - TextureImporter::ImportTexture2D");
			data.Data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 0);
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
		texture->Handle = AssetHandle();
		data.Release();
		return texture;
	}

	Ref<Texture2D> TextureImporter::LoadTexture2D(const Buffer buffer)
	{
		ENGINE_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		Buffer data;
		{
			ENGINE_PROFILE_SCOPE("stbi_load - TextureImporter::ImportTexture2D");
			data.Data = stbi_load_from_memory(buffer.Data, buffer.Size, &width, &height, &channels, 0);
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
		texture->Handle = AssetHandle();
		data.Release();
		return texture;
	}

	void TextureImporter::SaveTexture2D(const AssetMetadata& metadata, const Ref<Asset>& asset)
	{
		TextureSerializer serializer = TextureSerializer();
		serializer.Serialize(metadata, asset);
	}

}
