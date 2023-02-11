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

		public Vector4(Vector2 xy, float z = 0.0f, float w = 0.0f)
		{
			X = xy.X;
			Y = xy.Y;
			Z = z;
			W = w;
		}

		public Vector4(Vector3 xyz, float w = 0.0f)
		{
			X = xyz.X;
			Y = xyz.Y;
			Z = xyz.Z;
			W = w;
		}

		public static implicit operator Vector4(Vector2 vec2) => new Vector4(vec2.X, vec2.Y);
		public static implicit operator Vector4(Vector3 vec3) => new Vector4(vec3.X, vec3.Y, vec3.Z);

		public override string ToString()
		{
			return $"({X}, {Y}, {Z}, {W})";
		}

		public float Magnitude => InternalCalls.Vector4_Magnitude(ref this);
		public float SqrMagnitude => InternalCalls.Vector4_SqrMagnitude(ref this);

		public Vector4 Normalize()
		{
			InternalCalls.Vector4_Normalize(out this);
			return this;
		}

		public static Vector4 operator +(Vector4 lhs, Vector4 rhs) => new Vector4(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z, lhs.W + rhs.W);
		public static Vector4 operator -(Vector4 lhs, Vector4 rhs) => new Vector4(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z, lhs.W - rhs.W);
		public static Vector4 operator *(Vector4 lhs, float rhs) => new Vector4(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs, lhs.W * rhs);
	}
}
