using Engine.Core;

namespace Engine.Scene
{
	public class Prefab
	{
		public readonly ulong ID;
		protected Prefab() { ID = 0; }
		internal Prefab(ulong id)
		{
			ID = id;
			Log.Trace($"{ID}");
		}
	}
}
