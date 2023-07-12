using Engine.Core;
using System.Runtime.InteropServices;

namespace Engine.Math
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector3
	{
		public float X, Y, Z;

		public Vector3()
		{
			X = Y = Z = 0.0f;
		}

		public Vector3(float x, float y, float z)
		{
			X = x; Y = y; Z = z;
		}

		public Vector3(Vector2 xy, float z = 0.0f)
		{
			X = xy.X; Y = xy.Y; Z = z;
		}

		public Vector3(Vector3 xyz)
		{
			X = xyz.X; Y = xyz.Y; Z = xyz.Z;
		}

		public Vector3(float scalar = 0.0f)
		{
			X = Y = Z = scalar;
		}

		public static Vector3 One => new Vector3(1.0f);
		public static Vector3 Zero => new Vector3(0.0f);
		public static Vector3 PositiveInfinity => new Vector3(float.PositiveInfinity);
		public static Vector3 NegativeInfinity => new Vector3(float.NegativeInfinity);
		public static Vector3 Right => new Vector3(1.0f, 0.0f, 0.0f);
		public static Vector3 Left => new Vector3(-1.0f, 0.0f, 0.0f);
		public static Vector3 Up => new Vector3(0.0f, 1.0f, 0.0f);
		public static Vector3 Down => new Vector3(0.0f, -1.0f, 0.0f);
		public static Vector3 Forward => new Vector3(0.0f, 0.0f, 1.0f);
		public static Vector3 Backward => new Vector3(0.0f, 0.0f, -1.0f);

		public static implicit operator Vector3(Vector2 vec2) => new Vector3(vec2);
		public static implicit operator Vector3(Vector4 vec4) => new Vector3(vec4);

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

		public Vector3 RotateAroundAxis(float angle, Vector3 axis)
		{
			InternalCalls.Vector3_RotateAroundAxis(out this, angle, ref axis);
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
