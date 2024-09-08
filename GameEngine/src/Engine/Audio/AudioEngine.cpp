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
		ma_sound* SoundInstances;
		uint32_t SoundInstancesIndex = 0;
		const uint32_t MAX_SOUND_INSTANCES = 8;
	};

	struct AudioEngineData
	{
		ma_resource_manager ResourceManager;

		ma_context Context;
		ma_device_info* PlaybackDeviceInfos;
		uint32_t PlaybackDeviceCount;

		std::vector<Ref<ma_engine>> Engines;
		std::vector<Ref<ma_device>> Devices;

		uint32_t EngineCount;
		uint32_t OutputDevice;

		std::unordered_map<AssetHandle, ma_sound*> AudioClips;
		std::unordered_map<AssetHandle, ma_audio_buffer_config> AudioBufferConfigs;
		std::unordered_map<UUID, ma_audio_buffer*> AudioBuffers;
		std::unordered_map<UUID, AudioSource> AudioSources;
		bool PlaybackPaused;

		float MasterVolume;
		bool IsMutedMaster;
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

		if (!s_AudioEngineData->AudioSources.at(entityID).SoundInstances)
			return false;

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
		ENGINE_CORE_TRACE("Engine Startup - Audio Engine Init");
		s_AudioEngineData = new AudioEngineData();

		// Config Resource Manager
		ma_resource_manager_config resourceManagerConfig = ma_resource_manager_config_init();
		resourceManagerConfig.decodedFormat = ma_format_f32;
		resourceManagerConfig.decodedChannels = 0;
		resourceManagerConfig.decodedSampleRate = 48000;

		resourceManagerConfig.jobThreadCount = 4;

		auto result = ma_resource_manager_init(&resourceManagerConfig, &s_AudioEngineData->ResourceManager);
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

		// Config Devices and Engines
		s_AudioEngineData->OutputDevice = 0;
		s_AudioEngineData->EngineCount = 0;
		for (uint32_t i = 0; i < s_AudioEngineData->PlaybackDeviceCount; i++)
		{
			ENGINE_CORE_INFO("Initializing {}: {}", i, s_AudioEngineData->PlaybackDeviceInfos[i].name);
			ma_device_config deviceConfig;
			ma_engine_config engineConfig;
			Ref<ma_engine> engine = s_AudioEngineData->Engines.emplace_back(CreateRef<ma_engine>());
			Ref<ma_device> device = s_AudioEngineData->Devices.emplace_back(CreateRef<ma_device>());

			// Config Device
			deviceConfig = ma_device_config_init(ma_device_type_playback);
			deviceConfig.playback.pDeviceID = &s_AudioEngineData->PlaybackDeviceInfos[i].id; // chosen device?
			deviceConfig.playback.format = s_AudioEngineData->ResourceManager.config.decodedFormat;
			deviceConfig.playback.channels = 0;
			deviceConfig.sampleRate = s_AudioEngineData->ResourceManager.config.decodedSampleRate;
			deviceConfig.dataCallback = data_callback;
			deviceConfig.pUserData = engine.get(); // engine count?

			result = ma_device_init(&s_AudioEngineData->Context, &deviceConfig, device.get()); // engine count?
			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_ERROR("Failed to initialize device for {}.", s_AudioEngineData->PlaybackDeviceInfos[i].name); // chosen device ?
				continue;
			}

			// Config Engine
			engineConfig = ma_engine_config_init();
			engineConfig.pDevice = device.get(); // engine count?
			engineConfig.pResourceManager = &s_AudioEngineData->ResourceManager;
			engineConfig.noAutoStart = MA_TRUE;

			result = ma_engine_init(&engineConfig, engine.get()); // engine count?
			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_ERROR("Failed to initialize engine for {}.", s_AudioEngineData->PlaybackDeviceInfos[i].name);  // chosen device ?
				ma_device_uninit(device.get()); // engine count?
				continue;
			}

			s_AudioEngineData->EngineCount++;
		}

		// Start Engines
		for (uint32_t i = 0; i < s_AudioEngineData->EngineCount; i++)
		{
			ENGINE_CORE_TRACE("Starting Engine: {}", i);
			result = ma_engine_start(s_AudioEngineData->Engines[i].get());
			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_WARN("Failed to start engine {}", i);
			}
		}

		// Start Default Device (stop other devices)
		for (uint32_t i = 0; i < s_AudioEngineData->PlaybackDeviceCount; i++)
		{
			ENGINE_CORE_TRACE("Stopping Device: {}", i);
			result = ma_device_stop(s_AudioEngineData->Devices[i].get());
			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_WARN("Failed to stop device {}", i);
			}

			// Set output device to default
			if (s_AudioEngineData->PlaybackDeviceInfos[i].isDefault)
			{
				ENGINE_CORE_TRACE("Starting Device: {}", i);
				s_AudioEngineData->OutputDevice = i;
				result = ma_device_start(s_AudioEngineData->Devices[s_AudioEngineData->OutputDevice].get());
				if (result != MA_SUCCESS)
				{
					ENGINE_CORE_WARN("Failed to start device {}", i);
				}
			}
		}
	}

	void AudioEngine::Shutdown()
	{
		if (!s_AudioEngineData)
			return;

		PausePlayback(true);
		
		for (const auto& [path, sound] : s_AudioEngineData->AudioClips)
			ma_sound_uninit(sound);

		for (const auto& [entityID, buffer] : s_AudioEngineData->AudioBuffers)
			ma_audio_buffer_uninit_and_free(buffer);

		for (uint32_t i = 0; i < s_AudioEngineData->EngineCount; i++)
		{
			ma_engine_uninit(s_AudioEngineData->Engines[i].get());
			ma_device_uninit(s_AudioEngineData->Devices[i].get());
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

		ma_device_stop(s_AudioEngineData->Devices[s_AudioEngineData->OutputDevice].get());

		s_AudioEngineData->OutputDevice = deviceNumber;
		ma_device_start(s_AudioEngineData->Devices[s_AudioEngineData->OutputDevice].get());
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
			ma_result result = ma_engine_set_volume(s_AudioEngineData->Engines[i].get(), linearVolume);
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
			ma_result result = ma_engine_set_volume(s_AudioEngineData->Engines[i].get(), volume);
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

		ma_sound* sound = new ma_sound();
		for (uint32_t i = 0; i < s_AudioEngineData->EngineCount; i++)
		{
			auto result = ma_sound_init_from_file(s_AudioEngineData->Engines[i].get(), path.generic_string().c_str(),
				MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE 
				| MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC
				//| MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM // TODO create stream implementation (only need to stream music or sounds over 2 seconds)
				, nullptr, nullptr, sound);

			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_WARN("Failed to initialize sound from file!");
				return;
			}
		}
		s_AudioEngineData->AudioClips[handle] = sound;
	}

	void AudioEngine::LoadSound(const Buffer& buffer, AssetHandle handle)
	{
		if (!s_AudioEngineData)
			return;

		if (!handle.IsValid())
			return;

		if (s_AudioEngineData->AudioBufferConfigs.find(handle) != s_AudioEngineData->AudioBufferConfigs.end())
		{
			ENGINE_CORE_WARN("Audio Buffer already loaded!");
			return;
		}

		for (uint32_t i = 0; i < s_AudioEngineData->EngineCount; i++)
		{
			ma_uint64 _size_in_frames = 0;
			void* _frames_out = nullptr;

			ma_decoder_config _decoder_config = ma_decoder_config_init(ma_format_f32, ma_engine_get_channels(s_AudioEngineData->Engines[i].get()), 48000);
			auto result = ma_decode_memory(buffer.Data, buffer.Size, &_decoder_config, &_size_in_frames, &_frames_out);
			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_WARN("Failed to decode sound from memory!");
				return;
			}

			ma_audio_buffer_config config = ma_audio_buffer_config_init(
				ma_format_f32,
				ma_engine_get_channels(s_AudioEngineData->Engines[i].get()),
				_size_in_frames,
				_frames_out,
				nullptr);
			config.sampleRate = s_AudioEngineData->Engines[i]->sampleRate;
			s_AudioEngineData->AudioBufferConfigs[handle] = config;
		}
	}

	void AudioEngine::PlaySound(UUID entityID, AssetHandle handle, const SoundParams& params)
	{
		if (!s_AudioEngineData)
			return;

		if (!handle.IsValid())
			return;

		AudioSource& source = s_AudioEngineData->AudioSources[entityID];
		if (source.SoundInstances == nullptr)
			source.SoundInstances = new ma_sound[source.MAX_SOUND_INSTANCES];
		source.SoundInstancesIndex++;

		// Reset Sound Instance Index
		if (source.SoundInstancesIndex >= source.MAX_SOUND_INSTANCES)
			ma_sound_uninit(&source.SoundInstances[source.SoundInstancesIndex % source.MAX_SOUND_INSTANCES]);

		// Create Sound Instance
		ma_sound* soundInstance = &source.SoundInstances[source.SoundInstancesIndex % source.MAX_SOUND_INSTANCES];
		for (uint32_t i = 0; i < s_AudioEngineData->EngineCount; i++)
		{
			if (s_AudioEngineData->AudioBufferConfigs.find(handle) != s_AudioEngineData->AudioBufferConfigs.end())
			{
				ma_audio_buffer* audioBuffer;
				ma_result result = MA_SUCCESS;
				if (s_AudioEngineData->AudioBuffers.find(entityID) == s_AudioEngineData->AudioBuffers.end())
				{
					result = ma_audio_buffer_alloc_and_init(&s_AudioEngineData->AudioBufferConfigs[handle], &audioBuffer);
					if (result != MA_SUCCESS)
					{
						ENGINE_CORE_WARN("Failed to allocate and initalize audio buffer based on config during playback!");
						ma_audio_buffer_uninit_and_free(audioBuffer);
						return;
					}
					s_AudioEngineData->AudioBuffers[entityID] = audioBuffer;
				}
				else
				{
					audioBuffer = s_AudioEngineData->AudioBuffers.at(entityID);
					/*
					result = ma_audio_buffer_init(&s_AudioEngineData->AudioBufferConfigs[handle], audioBuffer);
					if (result != MA_SUCCESS)
					{
						ENGINE_CORE_WARN("Failed to initialize audio buffer based on config during playback!");
						ma_audio_buffer_uninit(audioBuffer);
						return;
					}
					*/
				}

				result = ma_sound_init_from_data_source(s_AudioEngineData->Engines[i].get(), audioBuffer,
					MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC
					, nullptr, soundInstance);

				if (result != MA_SUCCESS)
				{
					ENGINE_CORE_WARN("Failed to initialize sound from audio buffer on playback!");
					return;
				}
			}
			else
			{
				if (s_AudioEngineData->AudioClips.find(handle) == s_AudioEngineData->AudioClips.end())
				{
					ENGINE_CORE_WARN("Sound not loaded and cannot be played!");
					return;
				}

				// Get Original Sound and Copy to Instance
				auto result = ma_sound_init_copy(s_AudioEngineData->Engines[i].get(), s_AudioEngineData->AudioClips.at(handle),
					MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE
					| MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC
					, nullptr, soundInstance);

				if (result != MA_SUCCESS)
				{
					ENGINE_CORE_WARN("Failed to initialize sound as copy of sound on playback!");
					return;
				}
			}
		}

		ma_sound_set_looping(soundInstance, params.Loop);
		ma_sound_set_volume(soundInstance, params.Volume);
		ma_sound_set_pitch(soundInstance, params.Pitch);

		// Play Sound Instance
		auto result = ma_sound_start(soundInstance);
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

		AudioSource& source = s_AudioEngineData->AudioSources.at(entityID);
		for (uint32_t i = 0; i < source.MAX_SOUND_INSTANCES; i++)
			ma_sound_stop(&source.SoundInstances[i]);
	}

	void AudioEngine::UninitSound(UUID entityID)
	{
		if (!CheckForAudioInstance(entityID))
			return;

		AudioSource source = s_AudioEngineData->AudioSources.at(entityID);
		ma_sound* sound = source.SoundInstances;
		for (uint32_t i = 0; i < source.MAX_SOUND_INSTANCES; i++)
			ma_sound_uninit(sound++);
	}

	void AudioEngine::EraseAudioSource(UUID entityID)
	{
		if (!CheckForAudioInstance(entityID))
			return;

		ma_audio_buffer_uninit_and_free(s_AudioEngineData->AudioBuffers[entityID]);
		s_AudioEngineData->AudioBuffers.erase(entityID);
		
		s_AudioEngineData->AudioSources.erase(entityID);
	}

	void AudioEngine::PausePlayback(bool pause)
	{
		if (!s_AudioEngineData)
			return;

		s_AudioEngineData->PlaybackPaused = pause;
	}

	bool AudioEngine::IsSoundPlaying(UUID entityID)
	{
		if (!CheckForAudioInstance(entityID))
			return false;

		AudioSource& source = s_AudioEngineData->AudioSources.at(entityID);
		for (uint32_t i = 0; i < source.MAX_SOUND_INSTANCES; i++)
		{
			if (ma_sound_is_playing(&source.SoundInstances[i]))
				return true;
		}

		return false;
	}

	bool AudioEngine::GetSoundLooping(UUID entityID)
	{
		if (!CheckForAudioInstance(entityID))
			return false;

		AudioSource& source = s_AudioEngineData->AudioSources.at(entityID);
		for (uint32_t i = 0; i < source.MAX_SOUND_INSTANCES; i++)
		{
			if (ma_sound_is_looping(&source.SoundInstances[i]))
				return true;
		}

		return false;
	}

	void AudioEngine::SetSoundLooping(UUID entityID, bool state)
	{
		if (!CheckForAudioInstance(entityID))
			return;

		AudioSource& source = s_AudioEngineData->AudioSources.at(entityID);
		for (uint32_t i = 0; i < source.MAX_SOUND_INSTANCES; i++)
			ma_sound_set_looping(&source.SoundInstances[i], state);
	}

	float AudioEngine::GetSoundVolume(UUID entityID)
	{
		if (!CheckForAudioInstance(entityID))
			return 0;

		AudioSource& source = s_AudioEngineData->AudioSources.at(entityID);
		for (uint32_t i = 0; i < source.MAX_SOUND_INSTANCES; i++)
			return ma_sound_get_volume(&source.SoundInstances[i]);

		return 0;
	}

	void AudioEngine::SetSoundVolume(UUID entityID, float volume)
	{
		if (!CheckForAudioInstance(entityID))
			return;

		AudioSource& source = s_AudioEngineData->AudioSources.at(entityID);
		for (uint32_t i = 0; i < source.MAX_SOUND_INSTANCES; i++)
			ma_sound_set_volume(&source.SoundInstances[i], volume);
	}

	float AudioEngine::GetSoundPitch(UUID entityID)
	{
		if (!CheckForAudioInstance(entityID))
			return 0;

		AudioSource& source = s_AudioEngineData->AudioSources.at(entityID);
		for (uint32_t i = 0; i < source.MAX_SOUND_INSTANCES; i++)
			return ma_sound_get_pitch(&source.SoundInstances[i]);

		return 0;
	}

	void AudioEngine::SetSoundPitch(UUID entityID, float pitch)
	{
		if (!CheckForAudioInstance(entityID))
			return;

		AudioSource& source = s_AudioEngineData->AudioSources.at(entityID);
		for (uint32_t i = 0; i < source.MAX_SOUND_INSTANCES; i++)
			ma_sound_set_pitch(&source.SoundInstances[i], pitch);
	}

}
