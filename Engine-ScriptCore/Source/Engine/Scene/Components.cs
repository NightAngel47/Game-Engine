using System.Runtime.InteropServices;
using Engine.Math;
using Engine.Core;

namespace Engine.Scene
{
	public abstract class Component
	{
		public RegisterComponentTypes ComponentType { get; protected set; } = RegisterComponentTypes.Invalid;
		public Entity ComponentEntity { get; protected set; } = null;
	}

	public class TagComponent : Component
	{
		[StructLayout(LayoutKind.Sequential)]
		public struct TagData
		{
			public string tag;
		}

		private TagData data;

		public string Tag { get { return data.tag; } set { data.tag = value; } }

		public TagComponent(Entity entity)
		{
			ComponentType = RegisterComponentTypes.TagComponent;
			ComponentEntity = entity;
		}

		public TagComponent(Entity entity, TagData data)
		{
			ComponentType = RegisterComponentTypes.TagComponent;
			ComponentEntity = entity;
			this.data = data;
		}
	}

	public class TransformComponent : Component
	{
		[StructLayout(LayoutKind.Sequential)]
		public struct TransformData
		{
			public Vector3 position;
			public Vector3 rotation;
			public Vector3 scale;
		}

		private TransformData data;

		public Vector3 Position
		{
			get => data.position;

			set
			{
				data.position = value;
				InternalCalls.TransformComponent_SetPosition(ComponentEntity.ID, ref data.position);
			}
		}

		public Vector3 Rotation 
		{ 
			get => data.rotation; 
			
			set 
			{ 
				data.rotation = value;
				InternalCalls.TransformComponent_SetRotation(ComponentEntity.ID, ref data.rotation);
			} 
		}

		public Vector3 Scale 
		{ 
			get => data.scale; 
			
			set 
			{
				data.scale = value;
				InternalCalls.TransformComponent_SetScale(ComponentEntity.ID, ref data.scale);
			}
		}

		public TransformComponent(Entity entity)
		{
			ComponentType = RegisterComponentTypes.TransformComponent;
			ComponentEntity = entity;
		}

		public TransformComponent(Entity entity, TransformData data)
		{
			ComponentType = RegisterComponentTypes.TransformComponent;
			ComponentEntity = entity;
			this.data = data;
		}
	}
}
