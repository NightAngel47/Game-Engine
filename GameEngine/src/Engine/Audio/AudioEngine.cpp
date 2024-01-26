#include "enginepch.h"
#include "Engine/Audio/AudioEngine.h"
#include "Engine/Project/Project.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
//https://miniaud.io/docs/manual/index.html

namespace Engine
{
	void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
	{
		(void)pInput;

		ma_engine_read_pcm_frames((ma_engine*)pDevice->pUserData, pOutput, frameCount, nullptr);
	}

	struct AudioEngineData
	{
		ma_resource_manager ResourceManager;

		ma_context Context;
		ma_device_info* PlaybackDeviceInfos;
		uint32_t PlaybackDeviceCount;

		ma_engine Engines[2];
		ma_device Devices[2];

		uint32_t EngineCount;

		std::unordered_map<AssetHandle, ma_sound> Sounds;

		uint32_t OutputDevice;
	};

	static AudioEngineData* s_AudioEngineData = nullptr;

	void AudioEngine::Init()
	{
		s_AudioEngineData = new AudioEngineData();

		ma_result result;

		// Config Resource Manager
		ma_resource_manager_config resourceManagerConfig = ma_resource_manager_config_init();
		resourceManagerConfig.decodedFormat = ma_format_f32;
		resourceManagerConfig.decodedChannels = 0;
		resourceManagerConfig.decodedSampleRate = 48000;

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
			ENGINE_CORE_INFO("    %d: %s\n", i, s_AudioEngineData->PlaybackDeviceInfos[i].name);
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

			// Set output device to default
			if (s_AudioEngineData->PlaybackDeviceInfos[i].isDefault)
			{
				s_AudioEngineData->OutputDevice = i;
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
	}

	void AudioEngine::Shutdown()
	{
		if (!s_AudioEngineData)
			return;
		
		for (auto& [path, sound] : s_AudioEngineData->Sounds)
		{
			ma_sound_uninit(&sound);
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

		for (uint32_t i = 0; i < s_AudioEngineData->EngineCount; i++)
		{
			ma_result result = ma_engine_set_volume(&s_AudioEngineData->Engines[i], linearVolume);
			if (result != MA_SUCCESS)
				ENGINE_CORE_WARN("Failed to set master volume!");
		}
	}

	void AudioEngine::LoadSound(const std::filesystem::path& path, const AssetHandle handle)
	{
		if (!s_AudioEngineData)
			return;

		if (s_AudioEngineData->Sounds.find(handle) != s_AudioEngineData->Sounds.end())
		{
			ENGINE_CORE_WARN("Audio file already loaded!");
			return;
		}

		ma_sound& sound = s_AudioEngineData->Sounds[handle];
		for (uint32_t i = 0; i < s_AudioEngineData->EngineCount; i++)
		{
			auto result = ma_sound_init_from_file(&s_AudioEngineData->Engines[i], path.generic_string().c_str(),
				MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE 
				| MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC
				| MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM
				, nullptr, nullptr, &sound);
			if (result != MA_SUCCESS)
			{
				ENGINE_CORE_WARN("Failed to initialize sound from file!");
				return;
			}
		}
	}

	void AudioEngine::PlaySound(const AssetHandle handle)
	{
		if (!s_AudioEngineData)
			return;

		if (s_AudioEngineData->Sounds.find(handle) == s_AudioEngineData->Sounds.end())
		{
			ENGINE_CORE_WARN("Sound not loaded and cannot be played!");
			return;
		}

		ma_sound& sound = s_AudioEngineData->Sounds.at(handle);
		auto result = ma_sound_start(&sound);
		if (result != MA_SUCCESS)
		{
			ENGINE_CORE_WARN("Failed to start sound!");
			return;
		}
	}
}
