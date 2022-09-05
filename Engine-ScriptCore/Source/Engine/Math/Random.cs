using Engine.Core;

namespace Engine.Math
{
	public class Random
	{
		// 0 - 1, max exclusive, uniform distribution
		public static float Float() => InternalCalls.Random_Float();
		public static float Float(uint seed) => InternalCalls.Random_Float_Seed(seed);

		// any int, max inclusive, uniform distribution
		public static int Int() => InternalCalls.Random_Int();
		public static int Int(uint seed) => InternalCalls.Random_Int_Seed(seed);

		// max exclusive, uniform distribution
		public static float Range(float min, float max) => InternalCalls.Random_Range_Float(min, max);
		public static float Range(float min, float max, uint seed) => InternalCalls.Random_Range_Float_Seed(min, max, seed);

		// max inclusive, uniform distribution
		public static int Range(int min, int max) => InternalCalls.Random_Range_Int(min, max);
		public static int Range(int min, int max, uint seed) => InternalCalls.Random_Range_Int_Seed(min, max, seed);
	}
}
