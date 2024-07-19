namespace Engine.Core
{
	public class AudioEngine
	{
		public static float MasterVolume
		{
			get => InternalCalls.AudioEngine_GetMasterVolume();
			set => InternalCalls.AudioEngine_SetMasterVolume(value);
		}

		public static bool IsMasterVolumeMuted() => InternalCalls.AudioEngine_IsMasterVolumeMuted();
		public static void SetMasterVolumeMuted(bool state) => InternalCalls.AudioEngine_SetMasterVolumeMuted(state);
		public static void ToggleMuteMasterVolume() => InternalCalls.AudioEngine_ToggleMuteMasterVolume();
	}
}
