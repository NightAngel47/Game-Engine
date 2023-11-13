#include "enginepch.h"
#include "Engine/Audio/AudioEngine.h"
#include "Engine/Project/Project.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
//https://miniaud.io/docs/manual/index.html

namespace Engine
{

	struct AudioEngineData
	{
		ma_engine Engine;
		std::unordered_map<std::string, ma_sound> Sounds;
	};

	static AudioEngineData* s_AudioEngineData = nullptr;

	void AudioEngine::Init()
	{
		s_AudioEngineData = new AudioEngineData();

		ma_result result;

		//ma_engine_config engineConfig = ma_engine_config_init();
		
		//result = ma_engine_init(&engineConfig, &s_AudioEngineData->Engine);
		result = ma_engine_init(nullptr, &s_AudioEngineData->Engine);
		if (result != MA_SUCCESS)
		{
			ENGINE_CORE_ASSERT("Failed to initialize Audio Engine!");
			return;
		}
	}

	void AudioEngine::Shutdown()
	{
		if (!s_AudioEngineData)
			return;
		
		for (auto& [path, sound] : s_AudioEngineData->Sounds)
		{
			ma_sound_uninit(&sound);
		}

		ma_engine_uninit(&s_AudioEngineData->Engine);

		delete s_AudioEngineData;
	}

	void AudioEngine::SetMasterVolume(float linearVolume)
	{
		if (!s_AudioEngineData)
			return;

		ma_result result = ma_engine_set_volume(&s_AudioEngineData->Engine, linearVolume);
		if (result != MA_SUCCESS)
			ENGINE_CORE_ASSERT("Failed to set master volume!");
	}

	void AudioEngine::LoadSound(const std::filesystem::path& path)
	{
		if (!s_AudioEngineData)
			return;

		ma_sound& sound = s_AudioEngineData->Sounds[path.filename().generic_string()];
		auto result = ma_sound_init_from_file(&s_AudioEngineData->Engine, path.generic_string().c_str(), 0, nullptr, nullptr, &sound);
		if (result != MA_SUCCESS)
		{
			ENGINE_CORE_ASSERT("Failed to initialize sound from file!");
			return;
		}
	}

	void AudioEngine::PlaySound(const std::filesystem::path& path)
	{
		if (!s_AudioEngineData)
			return;

		if (s_AudioEngineData->Sounds.find(path.filename().generic_string()) == s_AudioEngineData->Sounds.end())
		{
			ENGINE_CORE_ERROR("Sound not loaded and cannot be played!");
			return;
		}

		ma_sound& sound = s_AudioEngineData->Sounds.at(path.filename().generic_string());
		auto result = ma_sound_start(&sound);
		if (result != MA_SUCCESS)
		{
			ENGINE_CORE_ASSERT("Failed to start sound!");
			return;
		}
	}
}
