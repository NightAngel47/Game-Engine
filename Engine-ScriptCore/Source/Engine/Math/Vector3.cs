using Engine.Core;
using System.Runtime.InteropServices;

namespace Engine.Math
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector3
	{
		public float X, Y, Z;

		public Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
		{
			X = x;
			Y = y;
			Z = z;
		}
		public Vector3(Vector2 xy, float z = 0.0f)
		{
			X = xy.X;
			Y = xy.Y;
			Z = z;
		}

		public static implicit operator Vector3(Vector2 vec2) => new Vector3(vec2.X, vec2.Y);
		public static implicit operator Vector3(Vector4 vec4) => new Vector3(vec4.X, vec4.Y, vec4.Z);

		public override string ToString()
		{
			return $"({X}, {Y}, {Z})";
		}

		public float Magnitude => InternalCalls.Vector3_Magnitude(ref this);
		public float SqrMagnitude => InternalCalls.Vector3_SqrMagnitude(ref this);

		public Vector3 Normalize()
		{
			InternalCalls.Vector3_Normalize(out this);
			return this;
		}

		public Vector3 Lerp(Vector3 a, Vector3 b, float t)
		{
			return new Vector3(Mathf.Lerp(a.X, b.X, t), Mathf.Lerp(a.Y, b.Y, t), Mathf.Lerp(a.Z, b.Z, t));
		}

		public static Vector3 operator +(Vector3 lhs, Vector3 rhs) => new Vector3(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z);
		public static Vector3 operator +(Vector3 lhs, Vector2 rhs) => new Vector3(lhs.X + rhs.X, lhs.Y + rhs.Y, 0.0f);
		public static Vector3 operator -(Vector3 lhs, Vector3 rhs) => new Vector3(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z);
		public static Vector3 operator *(Vector3 lhs, Vector3 rhs) => new Vector3(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z);
		public static Vector3 operator *(Vector3 lhs, float rhs) => new Vector3(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs);
	}
}
