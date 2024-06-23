#include "enginepch.h"
#include "Engine/Audio/AudioEngine.h"
#include "Engine/Project/Project.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
//https://miniaud.io/docs/manual/index.html

namespace Engine
{

	struct AudioSource
	{
		ma_sound SoundInstances[4];
		uint32_t SoundInstancesIndex = 0;
		const uint32_t MAX_SOUND_INSTANCES = 4;
	};

	struct AudioEngineData
	{
		ma_resource_manager ResourceManager;

		ma_context Context;
		ma_device_info* PlaybackDeviceInfos;
		uint32_t PlaybackDeviceCount;

		ma_engine Engines[2];
		ma_device Devices[2];

		uint32_t EngineCount;

		std::unordered_map<AssetHandle, ma_sound> AudioClips;
		std::unordered_map<UUID, AudioSource> AudioSources;
		bool PlaybackPaused;

		float MasterVolume;
		bool IsMutedMaster;

		uint32_t OutputDevice;
	};

	static AudioEngineData* s_AudioEngineData = nullptr;

	bool CheckForAudioInstance(UUID entityID)
	{
		if (!s_AudioEngineData)
			return false;

		if (!entityID.IsValid())
			return false;

		if (s_AudioEngineData->AudioSources.find(entityID) == s_AudioEngineData->AudioSources.end())
		{
			ENGINE_CORE_WARN("Audio Source doesn't have instance!");
			return false;
		}

		return true;
	}

	void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
	{
		(void)pInput;

		if (!s_AudioEngineData->PlaybackPaused)
		{
			ma_engine_read_pcm_frames((ma_engine*)pDevice->pUserData, pOutput, frameCount, nullptr);
		}
	}

	void AudioEngine::Init()
	{
		s_AudioEngineData = new AudioEngineData();

		ma_result result;

		// Config Resource Manager
		ma_resource_manager_config resourceManagerConfig = ma_resource_manager_config_init();
		resourceManagerConfig.decodedFormat = ma_format_f32;
		resourceManagerConfig.decodedChannels = 0;
		resourceManagerConfig.decodedSampleRate = 48000;

		resourceManagerConfig.jobThreadCount = 4;

		result = ma_resource_manager_init(&resourceManagerConfig, &s_AudioEngineData->ResourceManager);
		if (result != MA_SUCCESS)
		{
			ENGINE_CORE_ERROR("Failed to initialize Resource Manager!");
			return;
		}

		// Setup Context
		result = ma_context_init(nullptr, 0, nullptr, &s_AudioEngineData->Context);
		if (result != MA_SUCCESS)
		{
			ENGINE_CORE_ERROR("Failed to initialize Context!");
			return;
		}

		result = ma_context_get_devices(&s_AudioEngineData->Context, &s_AudioEngineData->PlaybackDeviceInfos, &s_AudioEngineData->PlaybackDeviceCount, nullptr, nullptr);
		if (result != MA_SUCCESS)
		{
			ENGINE_CORE_ERROR("Failed to enumerate playback devices!");
			return;
		}

		// Log available devices
		for (uint32_t i = 0; i < s_AudioEngineData->PlaybackDeviceCount; i++)
		{
			ENGINE_CORE_INFO("{}: {}", i, s_AudioEngineData->PlaybackDeviceInfos[i].name);
		}

		// Config Devices and Engines
		s_AudioEngineData->OutputDevice = 0;
		s_AudioEngineData->EngineCount = 0;
		for (uint32_t i = 0; i < s_AudioEngineData->PlaybackDeviceCount; i++)
		{
			ma_device_config deviceConfig;
			ma_engine_config engineConfig;

			// Config Device
			deviceConfig = ma_device_config_init(ma_device_type_playback);
			deviceConfig.playback.pDeviceID = &s_AudioEngineData->PlaybackDeviceInfos[i].id; // chosen device?
			deviceConfig.playback.format = s_AudioEngineData->ResourceManager.config.decodedFormat;
			deviceConfig.playback.channels = 0;
			deviceConfig.sampleRate = s_AudioEngineData->ResourceManager.config.decodedSampleRate;
			deviceConfig.dataCallback = data_callback;
			deviceConfig.pUserData = &s_AudioEngineData->Engines[s_AudioEngineData->EngineCount]; // engine count?

			result = ma_device_init(&s_AudioEngineData->Context, &deviceConfig, &s_AudioEngineData->Devices[s_AudioEngineData->EngineCount]); // engine count?
			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_ERROR("Failed to initialize device for {}.", s_AudioEngineData->PlaybackDeviceInfos[i].name); // chosen device ?
				return;
			}

			// Config Engine
			engineConfig = ma_engine_config_init();
			engineConfig.pDevice = &s_AudioEngineData->Devices[s_AudioEngineData->EngineCount]; // engine count?
			engineConfig.pResourceManager = &s_AudioEngineData->ResourceManager;
			engineConfig.noAutoStart = MA_TRUE;

			result = ma_engine_init(&engineConfig, &s_AudioEngineData->Engines[s_AudioEngineData->EngineCount]); // engine count?
			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_ERROR("Failed to initialize engine for {}.", s_AudioEngineData->PlaybackDeviceInfos[i].name);  // chosen device ?
				ma_device_uninit(&s_AudioEngineData->Devices[s_AudioEngineData->EngineCount]); // engine count?
				return;
			}

			s_AudioEngineData->EngineCount++;
		}

		// Start Engines
		for (uint32_t i = 0; i < s_AudioEngineData->EngineCount; i++)
		{
			result = ma_engine_start(&s_AudioEngineData->Engines[i]);
			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_WARN("Failed to start engine {}", i);
			}
		}

		// Start Default Device (stop other devices)
		for (uint32_t i = 0; i < s_AudioEngineData->PlaybackDeviceCount; i++)
		{
			ma_device_stop(&s_AudioEngineData->Devices[i]);

			// Set output device to default
			if (s_AudioEngineData->PlaybackDeviceInfos[i].isDefault)
			{
				s_AudioEngineData->OutputDevice = i;
				ma_device_start(&s_AudioEngineData->Devices[s_AudioEngineData->OutputDevice]);
			}
		}
	}

	void AudioEngine::Shutdown()
	{
		if (!s_AudioEngineData)
			return;
		
		for (auto& [path, sound] : s_AudioEngineData->AudioClips)
		{
			ma_sound_uninit(&sound);
		}

		for (auto& [entityID, source] : s_AudioEngineData->AudioSources)
		{
			for (auto& sound : source.SoundInstances)
			{
				ma_sound_uninit(&sound);
			}
		}

		for (uint32_t i = 0; i < s_AudioEngineData->EngineCount; i++)
		{
			ma_engine_uninit(&s_AudioEngineData->Engines[i]);
			ma_device_uninit(&s_AudioEngineData->Devices[i]);
		}

		ma_context_uninit(&s_AudioEngineData->Context);

		ma_resource_manager_uninit(&s_AudioEngineData->ResourceManager);

		delete s_AudioEngineData;
	}

	void AudioEngine::SetOutputDevice(uint32_t deviceNumber)
	{
		if (!s_AudioEngineData)
			return;

		if (deviceNumber >= s_AudioEngineData->EngineCount)
		{
			ENGINE_CORE_WARN("Device number out of range!");
			return;
		}

		ma_device_stop(&s_AudioEngineData->Devices[s_AudioEngineData->OutputDevice]);

		s_AudioEngineData->OutputDevice = deviceNumber;
		ma_device_start(&s_AudioEngineData->Devices[s_AudioEngineData->OutputDevice]);
	}

	uint32_t AudioEngine::GetOutputDevice()
	{
		if (!s_AudioEngineData)
			return 0;

		return s_AudioEngineData->OutputDevice;
	}

	uint32_t AudioEngine::GetTotalOutputDevices()
	{
		if (!s_AudioEngineData)
			return 0;

		return s_AudioEngineData->EngineCount;
	}

	char* AudioEngine::GetDeviceName(uint32_t deviceNumber)
	{
		if (!s_AudioEngineData)
			return nullptr;

		if (deviceNumber >= s_AudioEngineData->EngineCount)
		{
			ENGINE_CORE_WARN("Device number out of range!");
			return nullptr;
		}

		return s_AudioEngineData->PlaybackDeviceInfos[deviceNumber].name;
	}

	void AudioEngine::SetMasterVolume(float linearVolume)
	{
		if (!s_AudioEngineData)
			return;

		s_AudioEngineData->MasterVolume = linearVolume;

		if (s_AudioEngineData->IsMutedMaster)
			return;

		for (uint32_t i = 0; i < s_AudioEngineData->EngineCount; i++)
		{
			ma_result result = ma_engine_set_volume(&s_AudioEngineData->Engines[i], linearVolume);
			if (result != MA_SUCCESS)
				ENGINE_CORE_WARN("Failed to set master volume!");
		}
	}

	float AudioEngine::GetMasterVolume()
	{
		if (!s_AudioEngineData)
			return 0;

		return s_AudioEngineData->MasterVolume;
	}

	bool AudioEngine::IsMasterVolumeMuted()
	{
		if (!s_AudioEngineData)
			return true;

		return s_AudioEngineData->IsMutedMaster;
	}

	void AudioEngine::SetMasterVolumeMuted(bool state)
	{
		if (!s_AudioEngineData)
			return;

		s_AudioEngineData->IsMutedMaster = state;

		float volume = s_AudioEngineData->IsMutedMaster ? 0 : s_AudioEngineData->MasterVolume;
		for (uint32_t i = 0; i < s_AudioEngineData->EngineCount; i++)
		{
			ma_result result = ma_engine_set_volume(&s_AudioEngineData->Engines[i], volume);
			if (result != MA_SUCCESS)
				ENGINE_CORE_WARN("Failed to set master volume!");
		}
	}

	void AudioEngine::ToggleMuteMasterVolume()
	{
		if (!s_AudioEngineData)
			return;

		SetMasterVolumeMuted(!s_AudioEngineData->IsMutedMaster);
	}

	void AudioEngine::LoadSound(const std::filesystem::path& path, AssetHandle handle)
	{
		if (!s_AudioEngineData)
			return;

		if (!handle.IsValid())
			return;

		if (s_AudioEngineData->AudioClips.find(handle) != s_AudioEngineData->AudioClips.end())
		{
			ENGINE_CORE_WARN("Audio file already loaded!");
			return;
		}

		ma_sound& sound = s_AudioEngineData->AudioClips[handle];
		for (uint32_t i = 0; i < s_AudioEngineData->EngineCount; i++)
		{
			auto result = ma_sound_init_from_file(&s_AudioEngineData->Engines[i], path.generic_string().c_str(),
				MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE 
				| MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC
				//| MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM // TODO create stream implementation (only need to stream music or sounds over 2 seconds)
				, nullptr, nullptr, &sound);

			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_WARN("Failed to initialize sound from file!");
				return;
			}
		}
	}

	void AudioEngine::LoadSound(const Buffer& buffer, AssetHandle handle)
	{
		if (!s_AudioEngineData)
			return;

		if (!handle.IsValid())
			return;

		if (s_AudioEngineData->AudioClips.find(handle) != s_AudioEngineData->AudioClips.end())
		{
			ENGINE_CORE_WARN("Audio Clip already loaded!");
			return;
		}

		ma_audio_buffer_config config = ma_audio_buffer_config_init(
			ma_format_f32,
			0,
			1000,
			buffer.Data,
			nullptr);

		ma_audio_buffer audioBuffer;
		{
			auto result = ma_audio_buffer_init_copy(&config, &audioBuffer);
			if (result != MA_SUCCESS) {
				ENGINE_CORE_WARN("Failed to initialize from memory!");
				return;
			}
		}

		ma_sound& sound = s_AudioEngineData->AudioClips[handle];
		for (uint32_t i = 0; i < s_AudioEngineData->EngineCount; i++)
		{
			auto result = ma_sound_init_from_data_source(&s_AudioEngineData->Engines[i], &audioBuffer,
				MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE
				| MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC
				//| MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM // TODO create stream implementation (only need to stream music or sounds over 2 seconds)
				, nullptr, &sound);

			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_WARN("Failed to initialize sound from audio buffer!");
				return;
			}
		}
	}

	void AudioEngine::PlaySound(UUID entityID, AssetHandle clip, const SoundParams& params)
	{
		if (!s_AudioEngineData)
			return;

		if (!clip.IsValid())
			return;

		if (s_AudioEngineData->AudioClips.find(clip) == s_AudioEngineData->AudioClips.end())
		{
			ENGINE_CORE_WARN("Sound not loaded and cannot be played!");
			return;
		}

		// Reset Sound Instance Index
		AudioSource& source = s_AudioEngineData->AudioSources[entityID];
		if (source.SoundInstancesIndex >= source.MAX_SOUND_INSTANCES)
		{
			ma_sound& sound = source.SoundInstances[source.SoundInstancesIndex % source.MAX_SOUND_INSTANCES];
			ma_sound_uninit(&sound);
		}

		// Create Sound Instance
		ma_sound& soundInstance = source.SoundInstances[source.SoundInstancesIndex % source.MAX_SOUND_INSTANCES];
		source.SoundInstancesIndex++;

		// Get Original Sound and Copy to Instance
		ma_sound& originalSound = s_AudioEngineData->AudioClips.at(clip);
		for (uint32_t i = 0; i < s_AudioEngineData->EngineCount; i++)
		{
			auto result = ma_sound_init_copy(&s_AudioEngineData->Engines[i], &originalSound, 
				MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE
				| MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC
				, nullptr, &soundInstance);
			
			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_WARN("Failed to initialize sound from sound!");
				return;
			}
		}

		ma_sound_set_looping(&soundInstance, params.Loop);

		ma_sound_set_volume(&soundInstance, params.Volume);

		ma_sound_set_pitch(&soundInstance, params.Pitch);

		// Play Sound Instance
		auto result = ma_sound_start(&soundInstance);
		if (result != MA_SUCCESS)
		{
			ENGINE_CORE_WARN("Failed to start sound!");
			return;
		}
	}

	void AudioEngine::StopSound(UUID entityID)
	{
		if (!CheckForAudioInstance(entityID))
			return;

		for (auto& sound : s_AudioEngineData->AudioSources.at(entityID).SoundInstances)
			ma_sound_stop(&sound);
	}

	void AudioEngine::PausePlayback(bool pause)
	{
		s_AudioEngineData->PlaybackPaused = pause;
	}

	bool AudioEngine::IsSoundPlaying(UUID entityID)
	{
		if (!CheckForAudioInstance(entityID))
			return false;

		AudioSource& source = s_AudioEngineData->AudioSources.at(entityID);
		ma_sound& soundInstance = source.SoundInstances[source.SoundInstancesIndex % source.MAX_SOUND_INSTANCES];
		return ma_sound_is_playing(&soundInstance);
	}

	bool AudioEngine::GetSoundLooping(UUID entityID)
	{
		if (!CheckForAudioInstance(entityID))
			return false;

		AudioSource& source = s_AudioEngineData->AudioSources.at(entityID);
		ma_sound& soundInstance = source.SoundInstances[source.SoundInstancesIndex % source.MAX_SOUND_INSTANCES];
		return ma_sound_is_looping(&soundInstance);
	}

	void AudioEngine::SetSoundLooping(UUID entityID, bool state)
	{
		if (!CheckForAudioInstance(entityID))
			return;

		for (auto& sound : s_AudioEngineData->AudioSources.at(entityID).SoundInstances)
			ma_sound_set_looping(&sound, state);
	}

	float AudioEngine::GetSoundVolume(UUID entityID)
	{
		if (!CheckForAudioInstance(entityID))
			return 0;

		AudioSource& source = s_AudioEngineData->AudioSources.at(entityID);
		ma_sound& soundInstance = source.SoundInstances[source.SoundInstancesIndex % source.MAX_SOUND_INSTANCES];
		return ma_sound_get_volume(&soundInstance);
	}

	void AudioEngine::SetSoundVolume(UUID entityID, float volume)
	{
		if (!CheckForAudioInstance(entityID))
			return;

		for (auto& sound : s_AudioEngineData->AudioSources.at(entityID).SoundInstances)
			ma_sound_set_volume(&sound, volume);
	}

	float AudioEngine::GetSoundPitch(UUID entityID)
	{
		if (!CheckForAudioInstance(entityID))
			return 0;

		AudioSource& source = s_AudioEngineData->AudioSources.at(entityID);
		ma_sound& soundInstance = source.SoundInstances[source.SoundInstancesIndex % source.MAX_SOUND_INSTANCES];
		return ma_sound_get_pitch(&soundInstance);
	}

	void AudioEngine::SetSoundPitch(UUID entityID, float pitch)
	{
		if (!CheckForAudioInstance(entityID))
			return;

		for (auto& sound : s_AudioEngineData->AudioSources.at(entityID).SoundInstances)
			ma_sound_set_pitch(&sound, pitch);
	}

}
