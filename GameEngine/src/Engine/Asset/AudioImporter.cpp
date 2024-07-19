#include "enginepch.h"
#include "Engine/Asset/AudioImporter.h"
#include "Engine/Project/Project.h"

namespace Engine
{
	Ref<AudioClip> AudioImporter::ImportAudioClip(AssetHandle handle, const AssetMetadata& metadata)
	{
		ENGINE_PROFILE_FUNCTION();

		Ref<AudioClip> audioClip = LoadAudioClip(Project::GetActiveAssetFileSystemPath(metadata.Path).string(), handle);
		audioClip->Handle = handle;
		return audioClip;
	}

	Ref<AudioClip> AudioImporter::ImportAudioClipFromPak(AssetHandle handle, const PakAssetEntry& pakEntry)
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

		Ref<AudioClip> audioClip = LoadAudioClip({fileData.data(), fileData.size()}, handle);
		audioClip->Handle = handle;
		return audioClip;
	}

	Ref<AudioClip> AudioImporter::LoadAudioClip(const std::filesystem::path& filepath, AssetHandle handle)
	{
		ENGINE_PROFILE_FUNCTION();

		Ref<AudioClip> audioClip = CreateRef<AudioClip>();
		audioClip->Handle = handle.IsValid() ? handle : AssetHandle();
		
		AudioEngine::LoadSound(filepath, audioClip->Handle);

		return audioClip;
	}

	Ref<AudioClip> AudioImporter::LoadAudioClip(const Buffer buffer, AssetHandle handle)
	{
		ENGINE_PROFILE_FUNCTION();

		Ref<AudioClip> audioClip = CreateRef<AudioClip>();
		audioClip->Handle = handle.IsValid() ? handle : AssetHandle();

		AudioEngine::LoadSound(buffer, audioClip->Handle);

		return audioClip;
	}

	//TODO: Remove? Reevaluate asset pipeline
	void AudioImporter::SaveAudioClip(const AssetMetadata& metadata, const Ref<Asset>& asset)
	{
		// TODO figure out what saving does
		ENGINE_CORE_WARN("No functionality exists here.");
	}

}
