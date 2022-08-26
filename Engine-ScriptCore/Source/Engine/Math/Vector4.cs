using Engine.Core;
using System.Runtime.InteropServices;

namespace Engine.Math
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector4
	{
		public float X, Y, Z, W;

		public Vector4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f)
		{
			X = x;
			Y = y;
			Z = z;
			W = w;
		}

		public float Magnitude => InternalCalls.Vector4_Magnitude(ref this);
		public float sqrMagnitude => InternalCalls.Vector4_sqrMagnitude(ref this);

		public static Vector4 operator +(Vector4 lhs, Vector4 rhs) => new Vector4(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z, lhs.W + rhs.W);
		public static Vector4 operator -(Vector4 lhs, Vector4 rhs) => new Vector4(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z, lhs.W - rhs.W);
	}
}
