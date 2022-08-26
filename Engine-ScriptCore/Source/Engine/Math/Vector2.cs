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

		public float Magnitude => InternalCalls.Vector2_Magnitude(this);
		public float sqrMagnitude => InternalCalls.Vector2_sqrMagnitude(this);

		public static Vector2 operator +(Vector2 lhs, Vector2 rhs) => new Vector2(lhs.X + rhs.X, lhs.Y + rhs.Y);
		public static Vector2 operator -(Vector2 lhs, Vector2 rhs) => new Vector2(lhs.X - rhs.X, lhs.Y - rhs.Y);
	}
}
