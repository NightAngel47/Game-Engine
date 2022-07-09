using System.Runtime.InteropServices;

namespace Engine.Math
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector3
	{
		private readonly float[] xyz = new float[3];

		public float X { get => xyz[0]; set => xyz[0] = value; }
		public float Y { get => xyz[1]; set => xyz[1] = value; }
		public float Z { get => xyz[2]; set => xyz[2] = value; }

		public Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
		{
			this.X = x;
			this.Y = y;
			this.Z = z;
		}

		public Vector3(float[] xyz)
		{
			this.xyz = xyz;
		}

		public static implicit operator float[](Vector3 vector3) => vector3.xyz;
		public static explicit operator Vector3(float[] floats) => new Vector3(floats);

		public static Vector3 operator +(Vector3 lhs, Vector3 rhs) => new Vector3(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z);
		public static Vector3 operator -(Vector3 lhs, Vector3 rhs) => new Vector3(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z);
	}
}
