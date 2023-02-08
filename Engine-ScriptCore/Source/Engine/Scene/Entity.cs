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
		protected virtual void OnStart() { }
		protected virtual void OnDestroy() { }
		protected virtual void OnUpdate(float ts) { }
		protected virtual void OnLateUpdate(float ts) { }

		protected virtual void OnTriggerEnter2D() { }
		protected virtual void OnTriggerExit2D() { }

		public bool HasComponent<T>() where T : Component, new()
		{
			System.Type componentType = typeof(T);
			return InternalCalls.Entity_HasComponent(ID, componentType);
		}

		public T AddComponent<T>() where T : Component, new()
		{
			System.Type componentType = typeof(T);
			InternalCalls.Entity_AddComponent(ID, componentType);
			return GetComponent<T>();
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

		public Entity CreateEntity(string name = "Entity")
		{
			ulong entityID = InternalCalls.Entity_CreateEntity(name);
			if (entityID == 0)
				return null;

			return new Entity(entityID);
		}

		public Entity CreateEntity(string name, Vector3 position, Vector3 rotation, Vector3 scale)
		{
			Entity entity = CreateEntity(name);

			if (entity != null)
			{
				TransformComponent transform = entity.GetComponent<TransformComponent>();
				transform.Position = position;
				transform.Rotation = rotation;
				transform.Scale = scale;
			}

			return entity;
		}

		public T As<T>() where T : Entity, new()
		{
			object instance = InternalCalls.Entity_GetScriptInstance(ID);
			return instance as T;
		}

		public void DestroyEntity()
		{
			InternalCalls.Entity_DestroyEntity(ID);
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
