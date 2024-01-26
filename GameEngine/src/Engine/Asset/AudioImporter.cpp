#include "enginepch.h"
#include "Engine/Asset/AudioImporter.h"
#include "Engine/Project/Project.h"

namespace Engine
{
	Ref<AudioClip> AudioImporter::ImportAudioClip(AssetHandle handle, const AssetMetadata& metadata)
	{
		ENGINE_PROFILE_FUNCTION();

		Ref<AudioClip> audioClip = LoadAudioClip(Project::GetActiveAssetFileSystemPath(metadata.Path).string());
		audioClip->Handle = handle;
		return audioClip;
	}

	Ref<AudioClip> AudioImporter::LoadAudioClip(const std::filesystem::path& filepath, AssetHandle handle)
	{
		ENGINE_PROFILE_FUNCTION();

		Ref<AudioClip> audioClip = CreateRef<AudioClip>();
		audioClip->Handle = AssetHandle();
		
		AudioEngine::LoadSound(filepath, audioClip->Handle);

		return audioClip;
	}

	void AudioImporter::SaveAudioClip(const AssetMetadata& metadata, const Ref<Asset>& asset)
	{
		// TODO figure out what saving does
		ENGINE_CORE_WARN("No functionality exists here.");
	}

}
