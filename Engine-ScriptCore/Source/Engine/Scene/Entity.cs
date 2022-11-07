using Engine.Core;
using Engine.Math;

namespace Engine.Scene
{
	public class Entity
	{
		public readonly ulong ID;
		public TransformComponent Transform => GetComponent<TransformComponent>();

		protected Entity() { ID = 0; }

		internal Entity(ulong id)
		{
			ID = id;
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

		/// <summary>
		/// Finds first entity in scene that matches the given name.
		/// Also, this is a slow method that should be avoided, but could be good for testing.
		/// </summary>
		/// <param name="name">The name of the Entity to get.</param>
		/// <returns>The Entity for the given name.</returns>
		public Entity FindEntityByName(string name)
		{
			ulong entityID = InternalCalls.Entity_FindEntityByName(name);
			if (entityID == 0)
				return null;

			return new Entity(entityID);
		}

		public T As<T>() where T : Entity, new()
		{
			object instance = InternalCalls.Entity_GetScriptInstance(ID);
			return instance as T;
		}

		public Vector3 Position
		{
			get
			{
				InternalCalls.TransformComponent_GetPosition(ID, out Vector3 result);
				return result;
			}
			set
			{
				InternalCalls.TransformComponent_SetPosition(ID, ref value);
			}
		}
	}
}
