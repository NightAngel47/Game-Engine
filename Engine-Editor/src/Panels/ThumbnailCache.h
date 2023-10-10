#pragma once
#include <Engine.h>

namespace Engine
{

	struct ThumbnailImage
	{
		uint64_t Timestamp;
		Ref<Texture2D> Image;
	};


	class ThumbnailCache
	{
	public:
		ThumbnailCache(Ref<Project> project);

		Ref<Texture2D> GetThumbnail(const AssetHandle handle);
		Ref<Texture2D> GetThumbnail(const std::filesystem::path& path);
	private:
		Ref<Project> m_Project;

		std::map<std::filesystem::path, ThumbnailImage> m_CachedImages;

		std::filesystem::path m_ThumbnailCachePath;
	};
}
