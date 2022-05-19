using System.Runtime.InteropServices;

namespace Engine.Math
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector2
	{
		private readonly float[] xy = new float[2];
		
		public float X { get => xy[0]; set => xy[0] = value; }
		public float Y { get => xy[1]; set => xy[1] = value; }

		public Vector2(float x = 0.0f, float y = 0.0f)
		{
			this.X = x;
			this.Y = y;
		}

		public Vector2(float[] xy)
		{
			this.xy = xy;
		}


		public static implicit operator float[](Vector2 vector2) => vector2.xy;
		public static explicit operator Vector2(float[] floats) => new Vector2(floats);

		public static Vector2 operator +(Vector2 lhs, Vector2 rhs) => new Vector2(lhs.X + rhs.X, lhs.Y + rhs.Y);
	}
}
