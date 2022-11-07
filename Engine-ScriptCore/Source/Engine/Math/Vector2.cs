using Engine.Core;
using System.Runtime.InteropServices;

namespace Engine.Math
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector2
	{
		public float X, Y;

		public Vector2(float x = 0.0f, float y = 0.0f)
		{
			X = x;
			Y = y;
		}

		public static implicit operator Vector2(Vector3 vec3) => new Vector2(vec3.X, vec3.Y);
		public static implicit operator Vector2(Vector4 vec4) => new Vector2(vec4.X, vec4.Y);

		public override string ToString()
		{
			return $"({X}, {Y})";
		}

		public float Magnitude => InternalCalls.Vector2_Magnitude(ref this);
		public float sqrMagnitude => InternalCalls.Vector2_sqrMagnitude(ref this);

		public static Vector2 operator +(Vector2 lhs, Vector2 rhs) => new Vector2(lhs.X + rhs.X, lhs.Y + rhs.Y);
		public static Vector2 operator -(Vector2 lhs, Vector2 rhs) => new Vector2(lhs.X - rhs.X, lhs.Y - rhs.Y);
	}
}
