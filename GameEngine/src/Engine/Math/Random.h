#pragma once
#include <random>

namespace Engine
{
	namespace Math
	{
		class Random
		{
		public:

			// 0 - 1, max exclusive, uniform distribution
			static float Float();
			static float Float(unsigned int seed);

			// any int, max inclusive, uniform distribution
			static int Int();
			static int Int(unsigned int seed);

			// max exclusive, uniform distribution
			static float Range(float min, float max);
			static float Range(float min, float max, unsigned int seed);

			// max inclusive, uniform distribution
			static int Range(int min, int max);
			static int Range(int min, int max, unsigned int seed);
		};
	}
}

