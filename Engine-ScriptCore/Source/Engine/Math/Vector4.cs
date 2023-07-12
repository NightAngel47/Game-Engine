using Engine.Core;
using System.Runtime.InteropServices;

namespace Engine.Math
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector4
	{
		public float X, Y, Z, W;

		public Vector4()
		{
			X = Y = Z = W = 0.0f;
		}

		public Vector4(float x, float y, float z, float w)
		{
			X = x; Y = y; Z = z; W = w;
		}

		public Vector4(Vector2 xy, float z = 0.0f, float w = 0.0f)
		{
			X = xy.X; Y = xy.Y; Z = z; W = w;
		}

		public Vector4(Vector3 xyz, float w = 0.0f)
		{
			X = xyz.X; Y = xyz.Y; Z = xyz.Z; W = w;
		}

		public Vector4(Vector4 xyzw)
		{
			X = xyzw.X; Y = xyzw.Y; Z = xyzw.Z; W = xyzw.W;
		}

		public Vector4(float scalar = 0.0f)
		{
			X = Y = Z = W = scalar;
		}

		public static Vector4 One => new Vector4(1.0f);
		public static Vector4 Zero => new Vector4(0.0f);
		public static Vector4 PositiveInfinity => new Vector4(float.PositiveInfinity);
		public static Vector4 NegativeInfinity => new Vector4(float.NegativeInfinity);

		public static implicit operator Vector4(Vector2 vec2) => new Vector4(vec2);
		public static implicit operator Vector4(Vector3 vec3) => new Vector4(vec3);

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

		public Vector4 RotateAroundAxis(float angle, Vector3 axis)
		{
			InternalCalls.Vector4_RotateAroundAxis(out this, angle, ref axis);
			return this;
		}

		public Vector4 Lerp(Vector4 a, Vector4 b, float t)
		{
			return new Vector4(Mathf.Lerp(a.X, b.X, t), Mathf.Lerp(a.Y, b.Y, t), Mathf.Lerp(a.Z, b.Z, t), Mathf.Lerp(a.W, b.W, t));
		}

		public static Vector4 operator +(Vector4 lhs, Vector4 rhs) => new Vector4(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z, lhs.W + rhs.W);
		public static Vector4 operator -(Vector4 lhs, Vector4 rhs) => new Vector4(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z, lhs.W - rhs.W);
		public static Vector4 operator *(Vector4 lhs, Vector4 rhs) => new Vector4(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z, lhs.W * rhs.W);
		public static Vector4 operator *(Vector4 lhs, float rhs) => new Vector4(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs, lhs.W * rhs);
	}
}
