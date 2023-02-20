#include "RandomGenerator.h"

using rg = RandomGenerator;

void rg::init()
{
	if (!initialized)
	{
		mt = std::mt19937(rnd_dev());
		initialized = true;
	}
}

void rg::init(uint32_t seed)
{
	if (!initialized)
	{
		mt = std::mt19937(seed);
		initialized = true;
	}
}

template < class T >
T rg::random(T min, T max)
{
	init();

	return T.default;
}

template<>
int rg::random(int min, int max)
{
	init();

	std::uniform_int_distribution dist(min, max);

	return dist(mt);
}

template<>
float rg::random(float min, float max)
{	
	init();

	std::uniform_real_distribution dist(min, max);

	return dist(mt);
}