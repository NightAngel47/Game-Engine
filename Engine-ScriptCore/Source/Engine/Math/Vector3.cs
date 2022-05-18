using System.Runtime.InteropServices;

namespace Engine.Math
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector3
	{
		private float[] xyz = new float[3];

		public float x { get => xyz[0]; set => xyz[0] = value; }
		public float y { get => xyz[1]; set => xyz[1] = value; }
		public float z { get => xyz[2]; set => xyz[2] = value; }

		public Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
		{
			this.x = x;
			this.y = y;
			this.z = z;
		}

		public Vector3(float[] xyz)
		{
			this.x = xyz[0];
			this.y = xyz[1];
			this.z = xyz[2];
		}
	}
}
