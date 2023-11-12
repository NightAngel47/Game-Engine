#pragma once

namespace Engine
{
	class AudioEngine
	{
	public:
		static void Init();
		static void Shutdown();

		// Set master volume to 0 for silence, 1 for 100% volume, and above 1 for amplified
		static void SetMasterVolume(float linearVolume);

		static void PlaySound(const std::filesystem::path& path);
	};
}
