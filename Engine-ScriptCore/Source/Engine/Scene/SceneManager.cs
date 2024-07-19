using Engine.Core;

namespace Engine.Scene
{
	public class SceneManager
	{
		public static void LoadScene(ulong handle) => InternalCalls.SceneManager_LoadSceneByHandle(handle);
		public static void LoadScene(string path) => InternalCalls.SceneManager_LoadSceneByPath(path);
	}
}
