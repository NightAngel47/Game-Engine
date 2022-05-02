using System.Runtime.InteropServices;

namespace Engine.Math
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector2
	{
		private float[] xy = new float[2];
		
		public float x { get => xy[0]; set => xy[0] = value; }
		public float y { get => xy[1]; set => xy[1] = value; }

		public Vector2(float x = 0.0f, float y = 0.0f)
		{
			this.x = x;
			this.y = y;
		}
	}
}
