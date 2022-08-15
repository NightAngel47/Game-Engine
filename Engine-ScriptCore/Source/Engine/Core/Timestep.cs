namespace Engine.Core
{
	public struct Timestep
	{
		public Timestep(float time)
		{
			Time = time;
		}

		public float Time { get; }

		public override string ToString()
		{
			return $"Timestep: {Time}";
		}

		public static float operator +(float a, Timestep b)
		{
			return a + b.Time;
		}

		public static float operator -(float a, Timestep b)
		{
			return a - b.Time;
		}

		public static float operator *(float a, Timestep b)
		{
			return a * b.Time;
		}

		public static float operator /(float a, Timestep b)
		{
			return a / b.Time;
		}
	}
}
