#include "enginepch.h"
#include "Engine/Math/Random.h"

namespace Engine
{
	namespace Math
	{
		static std::mt19937 RandomEngine(unsigned int seed)
		{
			return std::mt19937(seed);
		}

		float Random::Float()
		{
			return Float(std::random_device()());
		}

		float Random::Float(unsigned int seed)
		{
			std::uniform_real_distribution<float> distribution;
			return distribution(RandomEngine(seed));
		}

		int Random::Int()
		{
			return Int(std::random_device()());
		}

		int Random::Int(unsigned int seed)
		{
			std::uniform_int_distribution<std::mt19937::result_type> distribution;
			return distribution(RandomEngine(seed));
		}

		float Random::Range(float min, float max)
		{
			return Range(min, max, std::random_device()());
		}


		float Random::Range(float min, float max, unsigned int seed)
		{
			std::uniform_real_distribution<float> distribution(min, max);
			return distribution(RandomEngine(seed));
		}


		int Random::Range(int min, int max)
		{
			return Range(min, max, std::random_device()());
		}


		int Random::Range(int min, int max, unsigned int seed)
		{
			std::uniform_int_distribution<std::mt19937::result_type> distribution(min, max);
			return distribution(RandomEngine(seed));
		}
	}
}
