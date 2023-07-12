using Engine.Core;
using System;
using System.Runtime.InteropServices;

namespace Engine.Math
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector2 : IEquatable<Vector2>, IComparable<Vector2>
	{
		public float X, Y;

		public Vector2()
		{
			X = Y = 0.0f;
		}

		public Vector2(float x, float y)
		{
			X = x; Y = y;
		}

		public Vector2(Vector2 xy)
		{
			X = xy.X; Y = xy.Y;
		}

		public Vector2(float scalar = 0.0f)
		{
			X = Y = scalar;
		}

		public static Vector2 One => new Vector2(1.0f);
		public static Vector2 Zero => new Vector2(0.0f);
		public static Vector2 PositiveInfinity => new Vector2(float.PositiveInfinity);
		public static Vector2 NegativeInfinity => new Vector2(float.NegativeInfinity);
		public static Vector2 Right => new Vector2(1.0f, 0.0f);
		public static Vector2 Left => new Vector2(-1.0f, 0.0f);
		public static Vector2 Up => new Vector2(0.0f, 1.0f);
		public static Vector2 Down => new Vector2(0.0f, -1.0f);

		public static implicit operator Vector2(Vector3 vec3) => new Vector2(vec3);
		public static implicit operator Vector2(Vector4 vec4) => new Vector2(vec4);

		public override string ToString()
		{
			return $"({X}, {Y})";
		}

		public bool Equals(Vector2 other)
		{
			return X == other.X && Y == other.Y;
		}

		public int CompareTo(Vector2 other)
		{
			if (SqrMagnitude < other.SqrMagnitude)
			{
				return -1;
			}
			else if (SqrMagnitude == other.SqrMagnitude)
			{
				return 0;
			}

			// other is greater than
			return 1;
		}

		public float Magnitude => InternalCalls.Vector2_Magnitude(ref this);
		public float SqrMagnitude => InternalCalls.Vector2_SqrMagnitude(ref this);

		public Vector2 Normalize()
		{
			InternalCalls.Vector2_Normalize(out this);
			return this;
		}

		public Vector2 Rotate(float angle)
		{
			InternalCalls.Vector2_RotateAroundAxis(out this, angle);
			return this;
		}

		public Vector2 Lerp(Vector2 a, Vector2 b, float t)
		{
			return new Vector2(Mathf.Lerp(a.X, b.X, t), Mathf.Lerp(a.Y, b.Y, t));
		}

		public static bool operator ==(Vector2 lhs, Vector2 rhs) => lhs.Equals(rhs);
		public static bool operator !=(Vector2 lhs, Vector2 rhs) => !lhs.Equals(rhs);

		public static Vector2 operator +(Vector2 lhs, Vector2 rhs) => new Vector2(lhs.X + rhs.X, lhs.Y + rhs.Y);
		public static Vector2 operator -(Vector2 lhs, Vector2 rhs) => new Vector2(lhs.X - rhs.X, lhs.Y - rhs.Y);
		public static Vector2 operator *(Vector2 lhs, Vector2 rhs) => new Vector2(lhs.X * rhs.X, lhs.Y * rhs.Y);
		public static Vector2 operator *(Vector2 lhs, float rhs) => new Vector2(lhs.X * rhs, lhs.Y * rhs);
	}
}
