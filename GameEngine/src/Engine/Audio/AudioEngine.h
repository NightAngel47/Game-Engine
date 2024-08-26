#pragma once
#include "Engine/Asset/Assets.h"

namespace Engine
{
	struct SoundParams
	{
		bool Loop = false;
		float Volume = 1;
		float Pitch = 0;
	};

	struct AudioClip : public Asset
	{
		static AssetType GetStaticType() { return AssetType::AudioClip; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }
	};

	class AudioEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void SetOutputDevice(uint32_t deviceNumber);
		static uint32_t GetOutputDevice();
		static uint32_t GetTotalOutputDevices();
		static char* GetDeviceName(uint32_t deviceNumber);

		// Set master volume to 0 for silence, 1 for 100% volume, and above 1 for amplified
		static void SetMasterVolume(float linearVolume);
		static float GetMasterVolume();
		static bool IsMasterVolumeMuted();
		static void SetMasterVolumeMuted(bool state);
		static void ToggleMuteMasterVolume();

		static void LoadSound(const std::filesystem::path& path, AssetHandle handle);
		static void LoadSound(const Buffer& buffer, AssetHandle handle);

		static void PlaySound(UUID entityID, AssetHandle clip, const SoundParams& params = {});
		static void StopSound(UUID entityID);

		static void PausePlayback(bool pause);

		static bool IsSoundPlaying(UUID entityID);

		static bool GetSoundLooping(UUID entityID);
		static void SetSoundLooping(UUID entityID, bool state);
		static float GetSoundVolume(UUID entityID);
		static void SetSoundVolume(UUID entityID, float volume);
		static float GetSoundPitch(UUID entityID);
		static void SetSoundPitch(UUID entityID, float pitch);
	};
}
