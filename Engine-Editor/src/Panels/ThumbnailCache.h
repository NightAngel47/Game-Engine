#pragma once
#include <Engine.h>
#include <queue>

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

		void OnUpdate();
	private:
		Ref<Project> m_Project;

		std::map<std::filesystem::path, ThumbnailImage> m_CachedImages;

		struct ThumbnailInfo
		{
			std::filesystem::path FullPath;
			std::filesystem::path AssetPath;
			uint64_t Timestamp;
		};
		std::queue<ThumbnailInfo> m_Queue;

		std::filesystem::path m_ThumbnailCachePath;
	};
}
