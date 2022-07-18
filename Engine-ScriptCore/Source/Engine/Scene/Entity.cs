using Engine.Core;

namespace Engine.Scene
{
	public abstract class Entity
	{
		public ulong ID { get; private set; }

		public Entity()
		{
			//Log.Info($"Entity Created with ID: {ID}");
		}

		protected abstract void OnCreate();
		protected abstract void OnDestroy();
		protected abstract void OnUpdate(float ts);

		public T GetComponent<T>() where T : Component
		{
			if (typeof(T).Equals(typeof(TagComponent)))
			{
				return GetComponent<T>(RegisterComponentTypes.TagComponent);
			}
			else if (typeof(T).Equals(typeof(TransformComponent)))
			{
				return GetComponent<T>(RegisterComponentTypes.TransformComponent);
			}
			else
			{
				return null;
			}
		}

		public T GetComponent<T>(RegisterComponentTypes componentType) where T : Component
		{
			switch(componentType)
			{
				case RegisterComponentTypes.TagComponent:
					InternalCalls.Entity_GetComponent(ID, out TagComponent.TagData tagComponentData);
					return new TagComponent(this, tagComponentData) as T;
				case RegisterComponentTypes.TransformComponent:
					InternalCalls.Entity_GetComponent(ID, out TransformComponent.TransformData transformComponentData);
					return new TransformComponent(this, transformComponentData) as T;
				case RegisterComponentTypes.Invalid:
				default:
					return null;
			}
		}
	}
}
