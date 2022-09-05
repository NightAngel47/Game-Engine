﻿using Engine.Core;

namespace Engine.Scene
{
	public abstract class Entity
	{
		public readonly ulong ID;
		public TransformComponent Transform => GetComponent<TransformComponent>();

		protected Entity() { ID = 0; }

		internal Entity(ulong id)
		{
			ID = id;
			Log.Info($"Entity Created with ID: {ID}");
		}

		protected virtual void OnCreate() { }
		protected virtual void OnDestroy() { }
		protected virtual void OnUpdate(float ts) { }

		public bool HasComponent<T>() where T : Component, new()
		{
			System.Type componentType = typeof(T);
			return InternalCalls.Entity_HasComponent(ID, componentType);
		}

		public T GetComponent<T>() where T : Component, new()
		{
			if (!HasComponent<T>())
			{
				return null;
			}

			return new T() { Entity = this };
		}
	}
}
