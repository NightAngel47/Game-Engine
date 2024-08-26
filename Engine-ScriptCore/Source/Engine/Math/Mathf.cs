
namespace Engine.Math
{
	public static class Mathf
	{
		//https://stackoverflow.com/questions/2683442/where-can-i-find-the-clamp-function-in-net
		public static T Clamp<T>(this T val, T min, T max) where T : System.IComparable<T>
		{
			if (val.CompareTo(min) < 0) return min;
			else if (val.CompareTo(max) > 0) return max;
			else return val;
		}

		public static float Lerp(float a, float b, float t)
		{
			return a + t * (b - a);
		}
	}
}
