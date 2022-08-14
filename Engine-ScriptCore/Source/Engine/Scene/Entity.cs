using System;
using Engine.Core;

namespace Engine.Scene
{
	public abstract class Entity
	{
		public readonly ulong ID;

		protected Entity() { ID = 0; }

		internal Entity(ulong id)
		{
			ID = id;
			Log.Info($"Entity Created with ID: {ID}");
		}

		protected abstract void OnCreate();
		protected abstract void OnDestroy();
		protected abstract void OnUpdate(Timestep ts);

		public bool HasComponent<T>() where T : Component, new()
		{
			Type componentType = typeof(T);
			return InternalCalls.Entity_HasComponent(ID, componentType);
		}

		public T GetComponent<T>() where T : Component, new()
		{
			if (!HasComponent<T>())
			{
				return null;
			}

			return new T();
		}
	}
}
