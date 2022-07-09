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
			//public float[] position;
			//public float[] rotation;
			//public float[] scale;

			public float posX, posY, posZ;
			public float rotX, rotY, rotZ;
			public float scaleX, scaleY, scaleZ;
		}

		private TransformData data;

		//public Vector3 Position { get { return new Vector3(data.position); } set { data.position[0] = value.x; data.position[1] = value.y; data.position[2] = value.z; } }
		public Vector3 Position
		{
			get => new Vector3(data.posX, data.posY, data.posZ);

			set
			{
				data.posX = value.X; 
				data.posY = value.Y; 
				data.posZ = value.Z;
				InternalCalls.TransformComponent_SetPosition(ComponentEntity.ID, out data.posX, out data.posY, out data.posZ);
			}
		}

		//public Vector3 Rotation { get { return new Vector3(data.rotation); } set { data.rotation[0] = value.x; data.rotation[1] = value.y; data.rotation[2] = value.z; } }
		public Vector3 Rotation { get => new Vector3(data.rotX, data.rotY, data.rotZ); set { data.rotX = value.X; data.rotY = value.Y; data.rotZ = value.Z; } }

		//public Vector3 Scale { get { return new Vector3(data.scale); } set { data.scale[0] = value.x; data.scale[1] = value.y; data.scale[2] = value.z; } }
		public Vector3 Scale { get => new Vector3(data.scaleX, data.scaleY, data.scaleZ); set { data.scaleX = value.X; data.scaleY = value.Y; data.scaleZ = value.Z; } }

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
