#include "enginepch.h"
#include "Engine/Audio/AudioEngine.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
//https://miniaud.io/docs/manual/index.html

namespace Engine
{

	struct AudioEngineData
	{
		ma_engine Engine;
		std::vector<ma_sound> Sounds;
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
		
		for (auto& sound : s_AudioEngineData->Sounds)
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

	void AudioEngine::PlaySound(const std::filesystem::path& path)
	{
		if (!s_AudioEngineData)
			return;

		if (ma_engine_play_sound(&s_AudioEngineData->Engine, path.generic_string().c_str(), nullptr) != MA_SUCCESS)
			ENGINE_CORE_ASSERT("Failed to play sound: {}", path.generic_string());

		/*
		ma_sound sound;

		{
			//ma_result result = ma_sound_init_from_file(&s_AudioEngineData->Engine, path.generic_string().c_str(), 0, nullptr, nullptr, &sound);
			//ma_result result = ma_sound_init_from_file(&s_AudioEngineData->Engine, path.generic_string().c_str(), MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, nullptr, nullptr, &sound);
			ma_result result = ma_sound_init_from_file(&s_AudioEngineData->Engine, path.generic_string().c_str(), MA_SOUND_FLAG_DECODE, nullptr, nullptr, &sound);
			//ma_result result = ma_sound_init_from_file(&s_AudioEngineData->Engine, path.generic_string().c_str(), MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE | MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC | MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM, nullptr, nullptr, &sound);
			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_ASSERT("Failed to load sound: {}", path.generic_string());
				return;
			}
		}

		s_AudioEngineData->Sounds.emplace_back(sound);

		{
			ma_result result = ma_sound_start(&sound);
			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_ASSERT("Failed to play sound: {}", path.generic_string());
				return;
			}
		}
		*/
	}
}
