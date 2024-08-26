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

		m_Queue.push({ fullPath, path, timestamp });
		return nullptr;
	}

	void ThumbnailCache::OnUpdate()
	{
		while (!m_Queue.empty())
		{
			const auto& info = m_Queue.front();
			if (m_CachedImages.find(info.AssetPath) != m_CachedImages.end())
			{
				auto& cachedImage = m_CachedImages.at(info.AssetPath);
				if (cachedImage.Timestamp == info.Timestamp)
				{
					m_Queue.pop();
					continue;
				}
			}
			Ref<Texture2D> thumbnail;
			AssetType fileType = m_Project->GetEditorAssetManager()->GetAssetTypeFromFileExtension(info.AssetPath.extension());
			switch (fileType)
			{
			case AssetType::Texture2D:
			{
				thumbnail = TextureImporter::LoadTexture2D(info.FullPath);
				float thumbnailHeight = m_ThumbnailSize * ((float)thumbnail->GetHeight() / (float)thumbnail->GetWidth());
				thumbnail->ChangeSize(m_ThumbnailSize, thumbnailHeight);
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
				auto& cachedImage = m_CachedImages[info.AssetPath];
				cachedImage.Image = thumbnail;
				cachedImage.Timestamp = info.Timestamp;
			}
			else
			{
				m_Queue.pop();
				continue;
			}
			m_Queue.pop();
			break;
		}
	}

}
