using System.Runtime.InteropServices;
using Engine.Math;

namespace Engine.Scene
{
	public abstract class Component
	{
		public RegisterComponentTypes ComponentType { get; protected set; }

		public Component()
		{
			ComponentType = RegisterComponentTypes.Invalid;
		}
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

		public TagComponent()
		{
			ComponentType = RegisterComponentTypes.TagComponent;
		}

		public TagComponent(TagData data)
		{
			ComponentType = RegisterComponentTypes.TagComponent;
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
		public Vector3 Position { get { return new Vector3(data.posX, data.posY, data.posZ); } set { data.posX = value.x; data.posY = value.y; data.posZ = value.z; } }
		//public Vector3 Rotation { get { return new Vector3(data.rotation); } set { data.rotation[0] = value.x; data.rotation[1] = value.y; data.rotation[2] = value.z; } }
		public Vector3 Rotation { get { return new Vector3(data.rotX, data.rotY, data.rotZ); } set { data.rotX = value.x; data.rotY = value.y; data.rotZ = value.z; } }
		//public Vector3 Scale { get { return new Vector3(data.scale); } set { data.scale[0] = value.x; data.scale[1] = value.y; data.scale[2] = value.z; } }
		public Vector3 Scale { get { return new Vector3(data.scaleX, data.scaleY, data.scaleZ); } set { data.scaleX = value.x; data.scaleY = value.y; data.scaleZ = value.z; } }

		public TransformComponent()
		{
			ComponentType = RegisterComponentTypes.TransformComponent;
		}

		public TransformComponent(TransformData data)
		{
			ComponentType = RegisterComponentTypes.TransformComponent;
			this.data = data;
		}
	}
}
