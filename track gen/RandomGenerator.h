#pragma once

#include <iostream>
#include <random>

class RandomGenerator
{
	public:
		//RandomGenerator();
		//RandomGenerator(uint32_t seed);

		static void init();
		static void init(uint32_t seed);

		template < class T >
		static T random(T min, T max);

		template <>
		static int random(int min, int max);

		template <>
		static float random(float min, float max);
	private:
		inline static std::random_device rnd_dev;
		inline static std::mt19937 mt;

		inline static bool initialized = false;
};