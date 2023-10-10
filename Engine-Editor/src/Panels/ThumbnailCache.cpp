#include "ThumbnailCache.h"
#include "Engine/Asset/TextureImporter.h"

#include <chrono>

namespace Engine
{
	ThumbnailCache::ThumbnailCache(Ref<Project> project)
		: m_Project(project)
	{
		m_ThumbnailCachePath = m_Project->GetActiveAssetDirectory() / "cache" / "Thumbnail.cache";
	}

	Ref<Texture2D> ThumbnailCache::GetThumbnail(const AssetHandle handle)
	{
		const auto& path = m_Project->GetEditorAssetManager()->GetAssetPath(handle);
		return GetThumbnail(path);
	}

	Ref<Texture2D> ThumbnailCache::GetThumbnail(const std::filesystem::path& path)
	{
		auto fullPath = m_Project->GetAssetFileSystemPath(path);
		std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(fullPath);
		uint64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(lastWriteTime.time_since_epoch()).count();

		if (m_CachedImages.find(path) != m_CachedImages.end())
		{
			auto& cachedImage = m_CachedImages.at(path);
			if (cachedImage.Timestamp == timestamp)
				return cachedImage.Image;
		}

		Ref<Texture2D> thumbnail;
		AssetType fileType = m_Project->GetEditorAssetManager()->GetAssetTypeFromFileExtension(path.extension());
		switch (fileType)
		{
		case AssetType::Texture2D:
		{
			thumbnail = TextureImporter::LoadTexture2D(fullPath);
			break;
		}
		case AssetType::Scene:
		case AssetType::Prefab:
		case AssetType::ScriptFile:
		case AssetType::None:
		default:
			thumbnail = nullptr;
			break;
		}

		if (thumbnail)
		{
			auto& cachedImage = m_CachedImages[path];
			cachedImage.Image = thumbnail;
			cachedImage.Timestamp = timestamp;
		}

		return thumbnail;
	}

}
