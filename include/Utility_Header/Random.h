#pragma once
#include <random>
#include <chrono>

namespace Random
{
	//Generated a seeded Mersenne Twister
	inline std::mt19937 generateMT()
	{
		//Use this to seed a seed sequence
		std::random_device rd{};

		//Create seed_seq with clock and 7 random numbers from std::random_device
		std::seed_seq ss{
			static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()), rd(), rd(), rd(), rd(),rd(), rd() };
		
		return std::mt19937{ ss };
	}

	inline std::mt19937 mt{ generateMT() };

	template<typename T>
	T get(T min, T max)
	{
		//Supported types: short, int , long ,long long, unsigned short, unsigned int ,unsigned long, or unsigned long long
		return std::uniform_int_distribution<T>{min, max}(mt);
	}
}