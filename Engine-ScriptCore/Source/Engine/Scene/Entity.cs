﻿using Engine.Core;
using Engine.Math;
using Engine.Physics;

namespace Engine.Scene
{
	public class Entity
	{
		public readonly ulong ID;
		public string Name => InternalCalls.Entity_GetName(ID);
		public TransformComponent Transform => GetComponent<TransformComponent>();

		protected Entity() { ID = 0; }

		internal Entity(ulong id)
		{
			ID = id;
		}

		public override string ToString() => ($"({Name} : {ID})");

		public override int GetHashCode() => base.GetHashCode();

		public override bool Equals(object obj)
		{
			//Check for null and compare run-time types.
			if ((obj == null) || !this.GetType().Equals(obj.GetType()))
			{
				return false;
			}
			else
			{
				Entity otherEntity = (Entity)(obj);
				return (ID == otherEntity.ID);
			}
		}

		public static bool operator ==(Entity lhs, Entity rhs) => (lhs.Equals(rhs));
		public static bool operator !=(Entity lhs, Entity rhs) => (!lhs.Equals(rhs));

		protected virtual void OnCreate() { }
		protected virtual void OnStart() { }
		protected virtual void OnDestroy() { }
		protected virtual void OnUpdate(float ts) { }
		protected virtual void OnLateUpdate(float ts) { }

		protected virtual void OnTriggerEnter2D(Physics2DContact contact2D) { }
		protected virtual void OnTriggerExit2D(Physics2DContact contact2D) { }
		protected virtual void OnCollisionEnter2D(Physics2DContact contact2D) { }
		protected virtual void OnCollisionExit2D(Physics2DContact contact2D) { }

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
			return CreateEntityFromID(InternalCalls.Entity_FindEntityByName(name));
		}

		public Entity CreateEntity(string name = "Entity")
		{
			return CreateEntityFromID(InternalCalls.Entity_CreateEntity(name));
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
			return InternalCalls.Entity_GetScriptInstance(ID) as T;
		}

		public void DestroyEntity()
		{
			InternalCalls.Entity_DestroyEntity(ID);
		}

		public Entity Parent
		{
			get => CreateEntityFromID(InternalCalls.Entity_GetParent(ID));
			set => InternalCalls.Entity_SetParent(ID, value.ID);
		}

		public Vector3 Position
		{
			get
			{
				InternalCalls.TransformComponent_GetPosition(ID, out Vector3 result);
				return result;
			}

			set => InternalCalls.TransformComponent_SetPosition(ID, ref value);
		}

		public Vector3 GetWorldPosition()
		{
			InternalCalls.Entity_GetWorldTransformPosition(ID, out Vector3 result);
			return result;
		}

		public Vector3 GetUIPosition()
		{
			InternalCalls.Entity_GetUITransformPosition(ID, out Vector3 result);
			return result;
		}

		private Entity CreateEntityFromID(ulong entityID)
		{
			if (entityID == 0)
				return null;

			return new Entity(entityID);
		}
	}
}
