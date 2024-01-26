#pragma once
#include "Engine/Asset/Assets.h"
#include "Engine/Audio/AudioEngine.h"

namespace Engine
{
	class AudioImporter
	{
	public:
		static Ref<AudioClip> ImportAudioClip(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<AudioClip> LoadAudioClip(const std::filesystem::path& filepath, AssetHandle handle = AssetHandle::INVALID());

		static void SaveAudioClip(const AssetMetadata& metadata, const Ref<Asset>& asset);
	};
}
