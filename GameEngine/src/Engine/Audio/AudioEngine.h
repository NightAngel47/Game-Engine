#pragma once
#include "Engine/Asset/Assets.h"

namespace Engine
{
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

		static void LoadSound(const std::filesystem::path& path, const AssetHandle handle);

		static void PlaySound(const AssetHandle handle);
	};
}
